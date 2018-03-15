/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "sound.h"

bool CSoundBank::m_UseSound = false;
CSound CSoundBank::m_Sound[SndCounter];

#define PW_AUDIO_FORMAT		AUDIO_S16
#define PW_AUDIO_FREQ		22050
#define PW_AUDIO_CHANNELS	1


//! Types and files of textures
struct SndFile {
	CSoundBank::SoundType	Type;
	const char*				FileName;
};

static const SndFile SoundFiles[] = {
	{ CSoundBank::SndClatz,	"clatz.wav"	},
};


void CSoundBank::Initialize()
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Audio initialization failed: %s\n", SDL_GetError());
	}
	else {
		SDL_AudioSpec soundAudiospec;
		soundAudiospec.freq = PW_AUDIO_FREQ;
		soundAudiospec.format = PW_AUDIO_FORMAT;
		soundAudiospec.channels = PW_AUDIO_CHANNELS;
		soundAudiospec.samples = 512;
		soundAudiospec.callback = OnFillBuffer;
		soundAudiospec.userdata = NULL;
		if (SDL_OpenAudio(&soundAudiospec, NULL) != 0)
			fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
		else {
			SDL_PauseAudio(1);

			assert(SndCounter == sizeof(SoundFiles) / sizeof(SoundFiles[0]));	//Don't forget sync counter and files!

			//Load the textures from files
			for (size_t i = 0; i < SndCounter; ++i) {
				char fileName[256];
				strcpy(fileName, DIR_GAMEDATA);
				strcat(fileName, SoundFiles[i].FileName);

				m_Sound[i].Load(fileName);
			}

			m_UseSound = true;
		}
	}
}


void CSoundBank::Free()
{
	for (size_t i = 0; i < SndCounter; ++i)
		m_Sound[i].Free();
}


void CSoundBank::Play(const SoundType /*type*/)
{
	//assert(type >= 0 && type < SndCounter);

	if (m_UseSound)
		SDL_PauseAudio(0);
}


void CSoundBank::OnFillBuffer(void* /*userdata*/, Uint8* stream, int len)
{
	const size_t i = SndClatz;
	Uint32 amount = (m_Sound[i].m_Length - m_Sound[i].m_Pos);
	if (amount > static_cast<Uint32>(len))
		amount = len;
	SDL_MixAudio(stream, &m_Sound[i].m_Data[m_Sound[i].m_Pos], amount, SDL_MIX_MAXVOLUME);
	m_Sound[i].m_Pos += amount;
	if (m_Sound[i].m_Pos >= m_Sound[i].m_Length) {
		m_Sound[i].m_Pos = 0;
		SDL_PauseAudio(1);
	}
}


void CSound::Load(const char* fileName)
{
	assert(fileName);

	SDL_AudioSpec wave;
	Uint8* data;
	Uint32 dlen;
	if (SDL_LoadWAV(fileName, &wave, &data, &dlen) == NULL)
		throw CException("Unable to load file ", fileName, ": ", SDL_GetError());

	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, PW_AUDIO_FORMAT, PW_AUDIO_CHANNELS, PW_AUDIO_FREQ);
	cvt.buf = static_cast<Uint8*>(malloc(dlen * cvt.len_mult));
	memcpy(cvt.buf, data, dlen);
	cvt.len = dlen;
	SDL_ConvertAudio(&cvt);
	SDL_FreeWAV(data);

	m_Data = cvt.buf;
	m_Pos = 0;
	m_Length = cvt.len_cvt;
}


void CSound::Free()
{
	if (m_Data)
		free(m_Data);

	m_Data = NULL;
	m_Pos = 0;
	m_Length = 0;
}
