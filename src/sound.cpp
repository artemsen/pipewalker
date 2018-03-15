/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2012 by Artem Senichev <artemsen@gmail.com>        *
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
#include "settings.h"

#define PW_AUDIO_FORMAT		AUDIO_S16
#define PW_AUDIO_FREQ		44100
#define PW_AUDIO_CHANNELS	2


sound::sound()
:	_curr_played(counter),
	_initialized(false)
{
}


sound& sound::instance()
{
	static sound i;
	return i;
}


void sound::initialize()
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Audio initialization failed: %s\n", SDL_GetError());
		return;
	}

	SDL_AudioSpec as;
	as.freq = PW_AUDIO_FREQ;
	as.format = PW_AUDIO_FORMAT;
	as.channels = PW_AUDIO_CHANNELS;
	as.samples = 512;
	as.callback = sound::sdl_on_fill_buffer;
	as.userdata = this;
	if (SDL_OpenAudio(&as, NULL) != 0) {
		fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
		return;
	}

	_initialized = true;

	SDL_PauseAudio(1);	//Set pause

	//Load sound files
	const char* snd_files[] = {
		PW_GAMEDATADIR "clatz.wav",
		PW_GAMEDATADIR "complete.wav",
	};
	assert(sizeof(snd_files) / sizeof(snd_files[0]) == counter);
	for (size_t i = 0; i < counter; ++i) {
		_bank[i].position = 0;
		SDL_AudioSpec wave_spec;
		Uint8* data = NULL;
		Uint32 dlen;
		if (SDL_LoadWAV(snd_files[i], &wave_spec, &data, &dlen) == NULL)
			fprintf(stderr, "Unable to load audio file %s: %s\n", snd_files[i], SDL_GetError());
		else {
			copy(data, data + dlen, back_inserter(_bank[i].data));
			SDL_FreeWAV(data);
		}
	}

	if (_bank[0].data.empty() && _bank[1].data.empty())
		_initialized = false;
}


void sound::play(const snd_type type)
{
	assert(type >= 0 && type < counter);

	sound& inst = instance();
	if (inst._initialized && settings::sound_mode()) {
		inst._curr_played = type;
		SDL_PauseAudio(0);
	}
}


void sound::sdl_on_fill_buffer(void* userdata, Uint8* stream, int len)
{
	sound* inst = reinterpret_cast<sound*>(userdata);
	if (!inst || inst->_curr_played < 0 || inst->_curr_played >= counter)
		return;

	wav& wv = inst->_bank[inst->_curr_played];

	Uint32 amount = static_cast<Uint32>(wv.data.size() - wv.position);
	if (amount > static_cast<Uint32>(len))
		amount = len;
	SDL_MixAudio(stream, &wv.data[wv.position], amount, SDL_MIX_MAXVOLUME);
	wv.position += amount;
	if (wv.position >= wv.data.size()) {
		wv.position = 0;
		inst->_curr_played = counter;
		SDL_PauseAudio(1);
	}
}
