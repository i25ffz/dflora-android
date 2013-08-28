/*
-----------------------------------------------------------------------------
This source file is part of Dflora (Dancing Flora) OpenGL|ES 1.1 version

Copyright (c) 2004-2005 Changzhi Li < richardzlee@163.com >

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free 
Software Foundation; either version 2 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
details.

You should have received a copy of the GNU Lesser General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 59 
Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

//#include <windows.h>
//#include <mmsystem.h>

#include <stdio.h>
#include <SLES/OpenSLES.h>
#include <assert.h>

#include "music.h"

//#pragma comment(lib, "winmm.lib")

static bool isInited = false;

/* engine interface */
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

/* output mix interfaces */
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

/* aux effect on the output mix */
static const SLEnvironmentalReverbSettings reverbSettings =
  SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

/* uri player interfaces */
static SLObjectItf uriPlayerObject = NULL;
static SLPlayItf uriPlayerPlay;
static SLSeekItf uriPlayerSeek;

void createEngine()
{
  SLresult result;

  // create engine
  result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
  assert(SL_RESULT_SUCCESS == result);

  // realize the engine
  result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);

  // get the engine interface
  result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
  assert(SL_RESULT_SUCCESS == result);

  // create output mix
  const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
  const SLboolean req[1] = {SL_BOOLEAN_FALSE};
  result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
  assert(SL_RESULT_SUCCESS == result);

  // realize the output mix
  result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
  assert(SL_RESULT_SUCCESS == result);

#if 0  
  // get the environmental reverb interface
  result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
		&outputMixEnvironmentalReverb);
  if (SL_RESULT_SUCCESS == result) {
	result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &reverbSettings);
  }
#endif
  // ignore unsuccessful result codes for env reverb
}

bool createUriAudioPlayer(const char* uri)
{
  SLresult result;

  // config audio source
  SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
  SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
  SLDataSource audioSrc = {&loc_uri, &format_mime};

  // config audio sink
  SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
  SLDataSink audioSnk = {&loc_outmix, NULL};

  // create audio player
  const SLInterfaceID ids[1] = {SL_IID_SEEK};
  const SLboolean req[1] = {SL_BOOLEAN_TRUE};
  result = (*engineEngine)->CreateAudioPlayer(engineEngine, &uriPlayerObject, &audioSrc, &audioSnk, 1, ids, req);
  assert(SL_RESULT_SUCCESS == result);

  // realize the player
  result = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
  if (SL_RESULT_SUCCESS != result) {
	(*uriPlayerObject)->Destroy(uriPlayerObject);
	uriPlayerObject = NULL;
	return false;
  }

  // get the play interface
  result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY, &uriPlayerPlay);
  assert(SL_RESULT_SUCCESS == result);

  // get the seek interface
  result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_SEEK, &uriPlayerSeek);
  assert(SL_RESULT_SUCCESS == result);

  // enable whole file looping
  result = (*uriPlayerSeek)->SetLoop(uriPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
  assert(SL_RESULT_SUCCESS == result);

  return true;
}

void setPlayingUriAudioPlayer(bool played)
{
  SLresult result;
  if (uriPlayerPlay != NULL) {
	result = (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, played ?
						SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
	assert(SL_RESULT_SUCCESS == result);
  }
}

int PlayMusic(const char *pathname)
{
	//PlaySound(pathname, NULL, SND_FILENAME|SND_ASYNC);

	// if not init, then create the OpenSL engine	
	if (!isInited)
	{
		createEngine();
		//after create engine, set to true
		isInited = true;
	}

	createUriAudioPlayer(pathname);
	setPlayingUriAudioPlayer(true);      // play the music

	return 0;
}

int StopMusic()
{
	//PlaySound(NULL, NULL, NULL);

	setPlayingUriAudioPlayer(false);    // pause the player

	return 0;
}
