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

#include "render.h"
#include "image.h"
#include "loading_xpm.h"
#include <GL/gl.h>

#define CHARACTERS_NUM 96
#define FONT_TEX_COL   16
#define FONT_TEX_ROW   8

#define PW_TSZ 64  ///< Tail cell size


render::render()
	: _loading(0)
{
	memset(_bank, 0, sizeof(_bank));
}


render::~render()
{
	free();
	if (_loading)
		glDeleteTextures(1, &_loading);
}


render& render::instance()
{
	static render i;
	return i;
}


void render::initialize()
{
	//GL attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Initialize OpenGL subsystem
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//We will use only arrays to draw
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	on_window_resize(PW_SCREEN_WIDTH, PW_SCREEN_HEIGHT);

	assert(check_GL_error());
}


bool render::load(const char* file_name)
{
	assert(file_name && *file_name);

	show_loading();

	free();

	image img;
	if (!img.load_PNG(file_name))
		return false;
	if (img.width() != 512 || img.height() != 448) {
		fprintf(stderr, "Incorrect image size, waiting for 512x448\n");
		return false;
	}

	//OpenGL need it
	img.flip_vertical();

	_bank[env_bkgr ] = create_texture(img.sub_image(0 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_REPEAT);
	_bank[cell_bkgr] = create_texture(img.sub_image(1 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_REPEAT);
	_bank[sender   ] = create_texture(img.sub_image(2 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);
	_bank[rcvr_psv ] = create_texture(img.sub_image(3 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);
	_bank[rcvr_act ] = create_texture(img.sub_image(4 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);
	_bank[lock     ] = create_texture(img.sub_image(5 * PW_TSZ, 6 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);
	_bank[exlposion] = create_texture(img.sub_image(6 * PW_TSZ, 6 * PW_TSZ + PW_TSZ / 2, PW_TSZ / 2, PW_TSZ / 2), GL_CLAMP);

	image th = img.sub_image(0 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[tube_hp] = create_texture(th, GL_CLAMP);
	_bank[tube_hs] = create_texture(make_tube_shadow(th), GL_CLAMP);
	_bank[tube_ha] = create_texture(img.sub_image(1 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);

	image ts = img.sub_image(2 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[tube_sp] = create_texture(ts, GL_CLAMP);
	_bank[tube_ss] = create_texture(make_tube_shadow(ts), GL_CLAMP);
	_bank[tube_sa] = create_texture(img.sub_image(3 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);

	image tc = img.sub_image(4 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[tube_cp] = create_texture(tc, GL_CLAMP);
	_bank[tube_cs] = create_texture(make_tube_shadow(tc), GL_CLAMP);
	_bank[tube_ca] = create_texture(img.sub_image(5 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);

	image tj = img.sub_image(6 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[tube_jp] = create_texture(tj, GL_CLAMP);
	_bank[tube_js] = create_texture(make_tube_shadow(tj), GL_CLAMP);
	_bank[tube_ja] = create_texture(img.sub_image(7 * PW_TSZ, 5 * PW_TSZ, PW_TSZ, PW_TSZ), GL_CLAMP);

	image bp = img.sub_image(0 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_prev] = create_texture(bp, GL_CLAMP);
	_bank[btn_prev_s] = create_texture(make_button_shadow(bp), GL_CLAMP);

	image bn = img.sub_image(1 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_next] = create_texture(bn, GL_CLAMP);
	_bank[btn_next_s] = create_texture(make_button_shadow(bn), GL_CLAMP);

	image bo = img.sub_image(2 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_ok] = create_texture(bo, GL_CLAMP);
	_bank[btn_ok_s] = create_texture(make_button_shadow(bo), GL_CLAMP);

	image bc = img.sub_image(3 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_cancel] = create_texture(bc, GL_CLAMP);
	_bank[btn_cancel_s] = create_texture(make_button_shadow(bc), GL_CLAMP);

	image br = img.sub_image(4 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_reset] = create_texture(br, GL_CLAMP);
	_bank[btn_reset_s] = create_texture(make_button_shadow(br), GL_CLAMP);

	image bs = img.sub_image(5 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_sett] = create_texture(bs, GL_CLAMP);
	_bank[btn_sett_s] = create_texture(make_button_shadow(bs), GL_CLAMP);

	image bc0 = img.sub_image(6 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_cb_off] = create_texture(bc0, GL_CLAMP);
	_bank[btn_cb_off_s] = create_texture(make_button_shadow(bc0), GL_CLAMP);

	image bc1 = img.sub_image(7 * PW_TSZ, 4 * PW_TSZ, PW_TSZ, PW_TSZ);
	_bank[btn_cb_on] = create_texture(bc1, GL_CLAMP);
	_bank[btn_cb_on_s] = create_texture(make_button_shadow(bc1), GL_CLAMP);

	_bank[env_title] = create_texture(img.sub_image(0, 3 * PW_TSZ, 8 * PW_TSZ, PW_TSZ), GL_CLAMP);

	image font_image = img.sub_image(0, 0, 8 * PW_TSZ, 3 * PW_TSZ);
	font_image.increase_canvas(256);
	_bank[font] = create_texture(font_image, GL_CLAMP);

	return true;
}


void render::on_window_resize(const int width, const int height) const
{
	glViewport(0, 0, width, height);
}


void render::draw_begin() const
{
	GLint gl_viewport[4];
	glGetIntegerv(GL_VIEWPORT, gl_viewport);
	const float wnd_width = static_cast<float>(gl_viewport[2]);
	const float wnd_height = static_cast<float>(gl_viewport[3]);
	const float vert_bkgr[] = { 0.0f, wnd_height, 0.0f, 0.0f, wnd_width, 0.0f, wnd_width, wnd_height };
	static const short tex_bkgr[] = { 0, 8, 0, 0, 6, 0, 6, 8 };
	static const unsigned short common_ind[] = { 0, 1, 2, 0, 2, 3 };
	static const short common_tex[] =          { 0, 1, 0, 0, 1, 0, 1, 1 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor4f(1.0f, 1.0, 1.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0.0 - PW_HALF_WIDTH,	                   0.0 + PW_HALF_WIDTH,
		0.0 - PW_HALF_WIDTH * PW_ASPECT_RATIO, 0.0 + PW_HALF_WIDTH * PW_ASPECT_RATIO,
		-1, 1);

	//Draw environment background
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, gl_viewport[2], 0, gl_viewport[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, _bank[env_bkgr]);
	glVertexPointer(2, GL_FLOAT, 0, vert_bkgr);
	glTexCoordPointer(2, GL_SHORT, 0, tex_bkgr);
	glDrawElements(GL_TRIANGLES, sizeof(common_ind) / sizeof(common_ind[0]), GL_UNSIGNED_SHORT, common_ind);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//Draw title
	static const float vert_title[] = { -5.0f, 6.1f, -5.0f, 5.1f, 5.0f, 5.1f, 5.0f, 6.1f };
	glBindTexture(GL_TEXTURE_2D, _bank[env_title]);
	glVertexPointer(2, GL_FLOAT, 0, vert_title);
	glTexCoordPointer(2, GL_SHORT, 0, common_tex);
	glDrawElements(GL_TRIANGLES, sizeof(common_ind) / sizeof(common_ind[0]), GL_UNSIGNED_SHORT, common_ind);

	assert(check_GL_error());
}


void render::draw_cell_field(const short cell_count, const float alpha) const
{
	//Draw cells
	static const unsigned short common_ind[] = { 0, 1, 2, 0, 2, 3 };
	static const float cell_vertex[] = { -5.0f, 5.0f, -5.0f, -5.0f, 5.0f, -5.0f, 5.0f, 5.0f };
	const short cell_texture[] =       { 0, cell_count, 0, 0, cell_count, 0, cell_count, cell_count };
	glColor4f(1.0f, 1.0, 1.0f, alpha);
	glBindTexture(GL_TEXTURE_2D, _bank[cell_bkgr]);
	glVertexPointer(2, GL_FLOAT, 0, cell_vertex);
	glTexCoordPointer(2, GL_SHORT, 0, cell_texture);
	glDrawElements(GL_TRIANGLES, sizeof(common_ind) / sizeof(common_ind[0]), GL_UNSIGNED_SHORT, common_ind);

	assert(check_GL_error());
}


void render::draw_end() const
{
		SDL_GL_SwapBuffers();

		assert(check_GL_error());

#ifndef NDEBUG
		//FPS counter
		static int fps_count = 0;
		static unsigned int fps_last_tick = SDL_GetTicks();
		++fps_count;
		if (SDL_GetTicks() - fps_last_tick >= 1000 /* ms */) {
			printf("FPS: %i\n", fps_count);
			fps_count = 0;
			fps_last_tick = SDL_GetTicks();
		}
#endif //NDEBUG
}


void render::draw(const img_type type, const float x, const float y, const float scale /*= 1.0f*/, const float alpha /*= 1.0f*/, const float angle /*= 0.0f*/)
{
	assert(type >= 0 && type < counter);

	static const float vert[] =         { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	static const short tex[] =          { 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned short ind[] = { 0, 1, 2, 0, 2, 3 };

	glColor4f(1.0f, 1.0, 1.0f, alpha);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(scale, scale, scale);
		glRotatef(angle, 0.0f, 0.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, _bank[type]);
		glVertexPointer(2, GL_FLOAT, 0, vert);
		glTexCoordPointer(2, GL_SHORT, 0, tex);
		glDrawElements(GL_TRIANGLES, sizeof(ind) / sizeof(ind[0]), GL_UNSIGNED_SHORT, ind);
	glPopMatrix();

	assert(check_GL_error());
}


void render::draw_text(const float x, const float y, const float scale, const float alpha, const char* text)
{
	assert(text);

	static const float symbol_width =  1.0f / FONT_TEX_COL;
	static const float symbol_height = 1.0f / FONT_TEX_ROW;

	static const float vertex[] =			{ 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f };
	static const unsigned short indices[] =	{ 0, 1, 2, 0, 2, 3 };

	glColor4f(1.0f, 1.0, 1.0f, alpha);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(scale, scale, scale);
		glBindTexture(GL_TEXTURE_2D, _bank[font]);
		glVertexPointer(2, GL_FLOAT, 0, vertex);

		while (*text) {
			unsigned char row = 1;
			unsigned char col = static_cast<unsigned char>(*text - ' ');
			while (col >= FONT_TEX_COL) {
				++row;
				col -= FONT_TEX_COL;
			}
			const float base_x = symbol_width * static_cast<float>(col);
			const float base_y = 1.0f - symbol_height * static_cast<float>(row);

 			const float tex[] = {
 				base_x, base_y + symbol_height,
 				base_x, base_y,
 				base_x + symbol_width, base_y,
 				base_x + symbol_width, base_y + symbol_height
 			};

			glTexCoordPointer(2, GL_FLOAT, 0, tex);
			glDrawElements(GL_TRIANGLES, (sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_SHORT, indices);

			glTranslatef(isupper(*text) ? 0.8f : 0.75f, 0.0f, 0.0f);			
			++text;
		}

	glPopMatrix();

	assert(check_GL_error());
}


void render::free()
{
	for (size_t i = 0; i < sizeof(_bank) / sizeof(_bank[0]); ++i) {
		if (_bank[i]) {
			glDeleteTextures(1, &_bank[i]);
			_bank[i] = 0;
		}
	}
}


void render::show_loading()
{
	if (_loading == 0) {
		image limg;
		limg.load_XPM(loading_xpm, sizeof(loading_xpm) / sizeof(loading_xpm[0]));
		limg.flip_vertical();
		_loading = create_texture(limg, GL_CLAMP);
	}
	static const float vert[] =         { -1.5f, 0.7f, -1.5f, -0.0f, 1.5f, -0.0f, 1.5f, 0.7f };
	static const short tex[] =          { 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned short ind[] = { 0, 1, 2, 0, 2, 3 };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		0.0 - PW_HALF_WIDTH,	                   0.0 + PW_HALF_WIDTH,
		0.0 - PW_HALF_WIDTH * PW_ASPECT_RATIO, 0.0 + PW_HALF_WIDTH * PW_ASPECT_RATIO,
		-1, 1);
	glBindTexture(GL_TEXTURE_2D, _loading);
	glVertexPointer(2, GL_FLOAT, 0, vert);
	glTexCoordPointer(2, GL_SHORT, 0, tex);
	glDrawElements(GL_TRIANGLES, sizeof(ind) / sizeof(ind[0]), GL_UNSIGNED_SHORT, ind);
	SDL_GL_SwapBuffers();

	assert(check_GL_error());
}


unsigned int render::create_texture(const image& img, const int mode) const
{
	assert(!(img.width() & (img.width() - 1)));	//Pow of 2
	assert(!(img.height() & (img.height() - 1)));	//Pow of 2

	GLuint id = 0;
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(img.width()), static_cast<GLsizei>(img.height()), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());

	assert(check_GL_error());

	return id;
}


image& render::make_tube_shadow(image& img) const
{
	img.gaussian_blur(5.0f);
	img.add_transparency(0.8f);
	img.fill_color(0, 0, 0);
	return img;
}


image& render::make_button_shadow(image& img) const
{
	img.increase_canvas(PW_TSZ + 16, PW_TSZ + 16);
	img.gaussian_blur(10.0f);
	img.resize(PW_TSZ, PW_TSZ);
	unsigned char r;
	unsigned char g;
	unsigned char b;
	img.average_color(r, g, b);
	img.fill_color(r, g, b);
	return img;
}


bool render::check_GL_error() const
{
	bool res = true;
	GLenum gl_err;
	while ((gl_err = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error: 0x%04x (%i)\n", gl_err, gl_err);
		res = false;
	}
	return res;
}
