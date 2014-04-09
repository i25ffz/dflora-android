/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifdef ANDROID

#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android/window.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "dflora", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "dflora", __VA_ARGS__))

#include "demo.h"
#include "timer.h"

static Demo g_Demo;
AAssetManager* g_assetMgr;

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;

    Real width = Product(ITOR(150), FTOR(1.0/32.0));
    Real height = Product(ITOR(150), FTOR(1.0/32.0));

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumr(-width, width, -height, height, ITOR(5), ITOR(500));
/*
    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
*/
    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        LOGW("No display.");
        return;
    }

    // Just fill the screen with a color.
    static TimeValue timelast = 0;
    TimeValue timecur = g_Timer.GetElapsedTime();
    TimeValue deltatime = (timecur-timelast)%100;
    timelast = timecur;

    g_Demo.Tick(deltatime);

    g_Demo.Render();

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;

    // destroy demo
    g_Demo.Destroy();
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->animating = 1;
        engine->state.x = AMotionEvent_getX(event, 0);
        engine->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            LOGI("handle APP_CMD_SAVE_STATE cmd...");
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("handle APP_CMD_INIT_WINDOW cmd...");
            if (engine->app->activity != NULL) {
                LOGI("set activity screen keep on...");
                ANativeActivity_setWindowFlags(engine->app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
            }

            // The window is being shown, get it ready.
            if (engine->app->window != NULL || engine->display == NULL) {
                engine_init_display(engine);
                g_Demo.Create();
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("handle APP_CMD_TERM_WINDOW cmd...");
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("handle APP_CMD_GAINED_FOCUS cmd...");
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
            }

            if (engine->app->window != NULL) {
                engine->animating = 1;
            }

            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("handle APP_CMD_LOST_FOCUS cmd...");
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);
    g_assetMgr = state->activity->assetManager;

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                            /*
                        LOGI("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                                */
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}

#else

#define SAMPLE_SURFACE_TYPE EGL_WINDOW_BIT
//#define SAMPLE_SURFACE_TYPE EGL_PIXMAP_BIT
//#define SAMPLE_SURFACE_TYPE EGL_PBUFFER_BIT

#define SAMPLE_PIXMAP_BITS 32   /* 16/32 */

#if !defined(SAMPLE_SURFACE_TYPE) || ((SAMPLE_SURFACE_TYPE != EGL_WINDOW_BIT) && (SAMPLE_SURFACE_TYPE != EGL_PIXMAP_BIT) && (SAMPLE_SURFACE_TYPE != EGL_PBUFFER_BIT))
#   error Define SAMPLE_SURFACE_TYPE properly.
#endif

#if defined (_MSC_VER)
#   pragma warning(disable : 4115) /* named type definition in parentheses */
#endif

#include <stdio.h>
#include "demo.h"
#include "timer.h"

/* EGL resources and Windows globals, defines, and prototypes */
//LONG WINAPI  MainWndProc (HWND, UINT, WPARAM, LPARAM);
//void         drawScene   (int width, int height);
//void updateState();
void perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

#define             SAMPLE_WINDOW_WIDTH     300
#define             SAMPLE_WINDOW_HEIGHT    300

static const char   g_szAppName[] = "Dflora - Dancing Flora";
static HWND         g_hWnd;
static Demo			g_Demo;

static EGLDisplay   g_EGLDisplay;
static EGLConfig    g_EGLConfig;
static EGLContext   g_EGLContext;
static EGLSurface   g_EGLWindowSurface;

LONG WINAPI MainWndProc (HWND hWnd,   UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT     ps;
    LONG            lRet = 1;

    switch (uMsg)
    {
    case WM_CREATE:
        {
            /* EGL Setup */
            static const EGLint s_configAttribs[] =
            {
#if (SAMPLE_PIXMAP_BITS == 16)
                EGL_RED_SIZE,       5,
                EGL_GREEN_SIZE,     5,
                EGL_BLUE_SIZE,      5,
#elif (SAMPLE_PIXMAP_BITS == 32)
                EGL_RED_SIZE,       8,
                EGL_GREEN_SIZE,     8,
                EGL_BLUE_SIZE,      8,
#endif
                EGL_ALPHA_SIZE,     8,
                EGL_DEPTH_SIZE,     24,
                EGL_STENCIL_SIZE,   8,
                EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,//SAMPLE_SURFACE_TYPE,
                EGL_NONE
            };

            EGLint numConfigs;
            EGLint majorVersion;
            EGLint minorVersion;


            g_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglGetConfigs(g_EGLDisplay, NULL, 0, &numConfigs);
            eglInitialize(g_EGLDisplay, &majorVersion, &minorVersion);
            eglChooseConfig(g_EGLDisplay, s_configAttribs, &g_EGLConfig, 1, &numConfigs);
            g_EGLContext = eglCreateContext(g_EGLDisplay, g_EGLConfig, NULL, NULL);

#if (SAMPLE_SURFACE_TYPE == EGL_WINDOW_BIT)
            g_EGLWindowSurface = eglCreateWindowSurface(g_EGLDisplay, g_EGLConfig, hWnd, NULL);
#endif
            eglMakeCurrent(g_EGLDisplay, g_EGLWindowSurface, g_EGLWindowSurface, g_EGLContext);
        }
		g_Demo.Create();
        break;

    case WM_PAINT:
        {
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_SIZE:
		{
//			int width = LOWORD(lParam);
//			int height = HIWORD(lParam);
			// Set the fustrum clipping planes

			Real w = Product(ITOR(150), FTOR(1.0/32.0));
			Real h = Product(ITOR(150), FTOR(1.0/32.0));
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustumr(-w, w, -h, h, ITOR(5), ITOR(500));
//			glViewport(0, 0, width, height);
//			float aspect = height ? (float)width/(float)height : 1.0f;
//			perspective(60.f, aspect, 10, 1000);
//			glMatrixMode(GL_MODELVIEW);
			break;
		}

    case WM_CLOSE:
        DestroyWindow (hWnd);
        break;

    case WM_DESTROY:
        /* Destroy all EGL resources */
        eglMakeCurrent(g_EGLDisplay, NULL, NULL, NULL);
        eglDestroyContext(g_EGLDisplay, g_EGLContext);
        eglDestroySurface(g_EGLDisplay, g_EGLWindowSurface);
        eglTerminate(g_EGLDisplay);
		g_Demo.Destroy();
        PostQuitMessage (0);
        break;

    case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case 'A':
			g_Demo.m_Camera.MoveSideward(FTOR(-1.0));
			break;
		case 'S':
			g_Demo.m_Camera.MoveForward(FTOR(-1.0));
			break;
		case 'W':
			g_Demo.m_Camera.MoveForward(FTOR(1.0));
			break;
		case 'D':
			g_Demo.m_Camera.MoveSideward(FTOR(1.0));
			break;
		case VK_PRIOR:
			g_Demo.m_Camera.Move(R_ZERO, R_ZERO, FTOR(1.f));
			break;
		case VK_NEXT:
			g_Demo.m_Camera.Move(R_ZERO, R_ZERO, FTOR(-1.f));
			break;
		case VK_LEFT:
			g_Demo.m_Camera.Turn(FTOR(0.01f), R_ZERO);
			break;
		case VK_RIGHT:
			g_Demo.m_Camera.Turn(FTOR(-0.01f), R_ZERO);
			break;
		case VK_UP:
			g_Demo.m_Camera.Turn(R_ZERO, FTOR(0.01f));
			break;
		case VK_DOWN:
			g_Demo.m_Camera.Turn(R_ZERO, FTOR(-0.01f));
			break;
		default:
			break;
		}
		break;

    default:
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

/* OpenGL code */

/*-------------------------------------------------------------------*//*!
 * \brief   Sets the perspective matrix to OpenGL.
 * \param   fovy    Vertical field-of-view.
 * \param   aspect  Aspect ratio.
 * \param   zNear   Distance to near plane.
 * \param   zFar    Distance to far plane.
 *//*-------------------------------------------------------------------*/
/*
void perspective (
    GLfloat fovy,
    GLfloat aspect,
    GLfloat zNear,
    GLfloat zFar)
{
    GLfloat xmin, xmax, ymin, ymax;

    ymax = zNear * (GLfloat)tan(fovy * 3.1415962f / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
}
*/
/*-------------------------------------------------------------------*//*!
 * \internal
 * \brief   Update state.
 * \param   width   Rendering width.
 * \param   height  Rendering height.
 *//*-------------------------------------------------------------------*/
/*
void updateState()
{
    static const GLfloat light_position[]   = { -50.f, 50.f, 200.f, 0.f };
    static const GLfloat light_ambient[]    = { 0.25f, 0.25f, 0.25f, 1.f };
    static const GLfloat light_diffuse[]    = { 1.f, 1.f, 1.f, 1.f };
    static const GLfloat material_spec[]    = { 0.f, 0.f, 0.f, 0.f };

    glLightfv           (GL_LIGHT0, GL_POSITION, light_position);
    glLightfv           (GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv           (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv           (GL_LIGHT0, GL_SPECULAR, light_diffuse);
    glMaterialfv        (GL_FRONT_AND_BACK, GL_SPECULAR, material_spec);

    glEnable            (GL_LIGHTING);
    glEnable            (GL_LIGHT0);
    glEnable            (GL_CULL_FACE);
//    glEnable            (GL_NORMALIZE);

    glShadeModel        (GL_SMOOTH);
    glClearColor        (0.1f, 0.2f, 0.1f, 0.f );
}
*/
/*-------------------------------------------------------------------*//*!
 * \brief   Main procedure.
 * \param   hInstance       Instance handle.
 * \param   hPrevInstance   Previous instance handle.
 * \param   lpCmdLine       Command line.
 * \param   nCmdShow        Show command.
 * \return  Returns exit status.
 *//*-------------------------------------------------------------------*/

int WINAPI WinMain (
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow)
{
    MSG             msg;
    WNDCLASS        wndclass;

    hPrevInstance;
    lpCmdLine;

    /* Register the frame class */
    wndclass.style         = 0;
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (hInstance, g_szAppName);
    wndclass.hCursor       = LoadCursor (NULL,IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wndclass.lpszMenuName  = g_szAppName;
    wndclass.lpszClassName = g_szAppName;

    if (!RegisterClass (&wndclass) )
        return FALSE;

    /* Create the frame */
    g_hWnd = CreateWindow (g_szAppName,
                          g_szAppName,
						  WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|
						  WS_MINIMIZEBOX|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
//                        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          SAMPLE_WINDOW_WIDTH,
                          SAMPLE_WINDOW_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    /* make sure window was created */
    if (!g_hWnd)
        return FALSE;

    /* adjust client rectangle size */
    {
        RECT    rc;
        DWORD   dwStyle;
        int     xp;
        int     yp;
        int     menuFlag;

        // change window style to popup
//      dwStyle = GetWindowStyle(hWnd);
        dwStyle = GetWindowLong(g_hWnd, GWL_STYLE);

        GetWindowRect(g_hWnd, &rc);
        xp = rc.left;   //(GetSystemMetrics(SM_CXSCREEN) - (rc.right-rc.left))/2;
        yp = rc.top;    //(GetSystemMetrics(SM_CYSCREEN) - (rc.bottom-rc.top))/2;

        menuFlag    = GetMenu(g_hWnd) != NULL;
        rc.top      = rc.left = 0;
        rc.right    = SAMPLE_WINDOW_WIDTH;
        rc.bottom   = SAMPLE_WINDOW_HEIGHT;
        AdjustWindowRect(&rc, dwStyle, menuFlag);

        SetWindowPos(g_hWnd, HWND_NOTOPMOST, xp, yp, rc.right - rc.left, rc.bottom - rc.top,
                         SWP_NOZORDER | SWP_NOACTIVATE );
    }

    /* show and update main window */
    ShowWindow (g_hWnd, nCmdShow);
    UpdateWindow (g_hWnd);

    /* Message loop */
    while(!g_Demo.IsFinished())
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
        {
			if (msg.message == WM_QUIT)
			{
				g_Demo.Finish();
			}
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
			{
				return 0;
			}

        }

		{
			RECT rc;
			GetClientRect(g_hWnd, &rc);
			int width = rc.right;
			int height = rc.bottom;
			glViewport(0, 0, width, height);
		}

	    static TimeValue timelast = 0;
		TimeValue timecur = g_Timer.GetElapsedTime();
		TimeValue deltatime = (timecur-timelast)%100;
		timelast = timecur;
/*
#ifdef _DEBUG
		char strDebug[256];
		sprintf(strDebug, "deltatime = %d \n", deltatime);
		OutputDebugString(strDebug);
#endif
*/
		g_Demo.Tick(deltatime);
		g_Demo.Render();
#if (SAMPLE_SURFACE_TYPE == EGL_WINDOW_BIT)
	    eglSwapBuffers(g_EGLDisplay, g_EGLWindowSurface);
#endif
		InvalidateRect(g_hWnd, NULL, FALSE);
    }
	return 0;
}

#endif
