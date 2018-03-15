/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
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
#ifdef PW_USE_WIN
	#include "buffer.h"
#endif //PW_USE_WIN

bool CSoundBank::m_SoundInitialized = false;
CSound CSoundBank::m_Sound[SndCounter];

#ifdef PW_USE_SDL
	#define PW_AUDIO_FORMAT		AUDIO_S16
	#define PW_AUDIO_FREQ		44100
	#define PW_AUDIO_CHANNELS	2

	CSoundBank::SoundType CSoundBank::m_CurrentSnd = CSoundBank::SndCounter;
#endif //PW_USE_SDL

//! Types and files of textures
struct SndFile {
	CSoundBank::SoundType	Type;
	const char*				FileName;
};

static const SndFile SoundFiles[] = {
	{ CSoundBank::SndClatz,		"clatz.wav"		},
	{ CSoundBank::SndComplete,	"complete.wav"	},
};


void CSoundBank::Load()
{
#if defined PW_USE_SDL		//SDL library
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Audio initialization failed: %s\n", SDL_GetError());
		return;
	}
	SDL_AudioSpec soundAudiospec;
	soundAudiospec.freq = PW_AUDIO_FREQ;
	soundAudiospec.format = PW_AUDIO_FORMAT;
	soundAudiospec.channels = PW_AUDIO_CHANNELS;
	soundAudiospec.samples = 512;
	soundAudiospec.callback = OnFillBuffer;
	soundAudiospec.userdata = NULL;
	if (SDL_OpenAudio(&soundAudiospec, NULL) != 0) {
		fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
		return;
	}

	SDL_PauseAudio(1);
#endif //PW_USE_SDL

	assert(SndCounter == sizeof(SoundFiles) / sizeof(SoundFiles[0]));	//Don't forget sync counter and files!

	//Load the sounds from files
	for (size_t i = 0; i < SndCounter; ++i) {
		char fileName[256];
		strcpy(fileName, DIR_GAMEDATA);
		strcat(fileName, SoundFiles[i].FileName);
		m_Sound[i].Load(fileName);
	}

	m_SoundInitialized = true;
}


void CSoundBank::Free()
{
	for (size_t i = 0; i < SndCounter; ++i)
		m_Sound[i].Free();
}


void CSoundBank::Play(const SoundType type)
{
	assert(type >= 0 && type < SndCounter);
#if defined PW_USE_SDL		//SDL library
	if (m_SoundInitialized) {
		SDL_PauseAudio(0);
		m_CurrentSnd = type;
	}
#elif defined PW_USE_WIN	//Microsoft Windows
	PlaySound(reinterpret_cast<LPCTSTR>(&m_Sound[type].m_Data.front()), NULL, SND_MEMORY | SND_ASYNC);
#endif
}


#ifdef PW_USE_SDL
void CSoundBank::OnFillBuffer(void* /*userdata*/, Uint8* stream, int len)
{
	if (m_CurrentSnd < 0 || m_CurrentSnd >= SndCounter)
		return;
	
	Uint32 amount = (m_Sound[m_CurrentSnd].m_Data.size() - m_Sound[m_CurrentSnd].m_Pos);
	if (amount > static_cast<Uint32>(len))
		amount = len;
	SDL_MixAudio(stream, &m_Sound[m_CurrentSnd].m_Data[m_Sound[m_CurrentSnd].m_Pos], amount, SDL_MIX_MAXVOLUME);
	m_Sound[m_CurrentSnd].m_Pos += amount;
	if (m_Sound[m_CurrentSnd].m_Pos >= m_Sound[m_CurrentSnd].m_Data.size()) {
		m_Sound[m_CurrentSnd].m_Pos = 0;
		m_CurrentSnd = SndCounter;
		SDL_PauseAudio(1);
	}
}
#endif //PW_USE_SDL


void CSound::Load(const char* fileName)
{
	assert(fileName);

#if defined PW_USE_SDL		//SDL library
	SDL_AudioSpec wave;
	Uint8* data;
	Uint32 dlen;
	if (SDL_LoadWAV(fileName, &wave, &data, &dlen) == NULL)
		throw CException("Unable to load file ", fileName, ": ", SDL_GetError());
	copy(data, data + dlen, back_inserter(m_Data));
	m_Pos = 0;
#elif defined PW_USE_WIN	//Microsoft Windows
	CBuffer buf;
	buf.Load(fileName);
	buf.Copy(m_Data);
#endif
}


void CSound::Free()
{
#if defined PW_USE_SDL		//SDL library
	m_Pos = 0;
#endif
	m_Data.clear();
}
