LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := dflora

# LOCAL_CFLAGS := -DANDROID_NDK

LOCAL_SRC_FILES := \
	butterfly.cpp \
	camera.cpp \
	flower.cpp \
	image.cpp  \
	music.cpp \
	terrain.cpp \
	timer.cpp \
	butterflygroup.cpp \
	demo.cpp \
	font.cpp \
	model.cpp \
	sky.cpp \
	texture.cpp \
	utility.cpp \
	file.cpp \
	application.cpp

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)