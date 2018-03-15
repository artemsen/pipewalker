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

#pragma once

//Base size/ratio
#define PW_BASE_WIDTH      10.4f
#define PW_HALF_WIDTH      (PW_BASE_WIDTH / 2.0f)
#define PW_ASPECT_RATIO    (static_cast<float>(PW_SCREEN_HEIGHT) / static_cast<float>(PW_SCREEN_WIDTH))

class image;


class render
{
private:
	render();
	~render();

public:
	//! Image types
	enum img_type {
		env_bkgr = 0, ///< Main environment background
		cell_bkgr,    ///< Cell background
		sender,       ///< Sender
		rcvr_psv,     ///< Passive receiver
		rcvr_act,     ///< Active receiver
		lock,         ///< Lock
		exlposion,    ///< Explosion particle
		tube_hs,      ///< Shadow of half tube
		tube_hp,      ///< Passive half tube
		tube_ha,      ///< Active half tube
		tube_ss,      ///< Shadow of straight tube
		tube_sp,      ///< Passive straight tube
		tube_sa,      ///< Active straight tube
		tube_cs,      ///< Shadow of curved tube
		tube_cp,      ///< Passive curved tube
		tube_ca,      ///< Active curved tube
		tube_js,      ///< Shadow of joiner tube
		tube_jp,      ///< Passive joiner tube
		tube_ja,      ///< Active joiner tube
		btn_prev,     ///< Button 'previous'
		btn_prev_s,   ///< Shadow of button 'previous'
		btn_next,     ///< Button 'next'
		btn_next_s,   ///< Shadow of button 'next'
		btn_ok,       ///< Button 'OK'
		btn_ok_s,     ///< Shadow of button 'OK'
		btn_cancel,   ///< Button 'Cancel'
		btn_cancel_s, ///< Shadow of button 'Cancel'
		btn_reset,    ///< Button 'reset'
		btn_reset_s,  ///< Shadow of button 'reset'
		btn_sett,     ///< Button 'settings'
		btn_sett_s,   ///< Shadow of button 'settings'
		btn_cb_on,    ///< Radio button (checkbox) 'On'
		btn_cb_on_s,  ///< Shadow of radio button (checkbox) 'On'
		btn_cb_off,   ///< Radio button (checkbox) 'Off'
		btn_cb_off_s, ///< Shadow of radio button (checkbox) 'Off'
		env_title,    ///< Main environment title
		font,         ///< Font texture
		counter       ///< Images counter
	};

	/**
	 * Get class instance
	 * \return class instance
	 */
	static render& instance();

	/**
	 * Initialize render
	 */
	void initialize();

	/**
	 * Initialize image bank (load images from file)
	 * \param file_name theme file name
	 * \return false if error
	 */
	bool load(const char* file_name);

	/**
	 * Window resize handler
	 * \param width new window width size
	 * \param height new window height size
	 */
	void on_window_resize(const int width, const int height) const;

	/**
	 * Begin draw
	 */
	void draw_begin() const;

	/**
	 * Draw cell field
	 * \param cell_count cell number
	 * \param alpha alpha channel in range [0.0f ... 1.0f]
	 */
	void draw_cell_field(const short cell_count, const float alpha) const;

	/**
	 * End draw - swap buffer
	 */
	void draw_end() const;

	/**
	 * Draw image
	 * \param type image type
	 * \param x an X coordinate of center
	 * \param y an Y coordinate of center
	 * \param scale scale coefficient
	 * \param alpha alpha channel in range [0.0f ... 1.0f]
	 * \param angle angle in degree
	 */
	void draw(const img_type type, const float x, const float y, const float scale = 1.0f, const float alpha = 1.0f, const float angle = 0.0f);

	/**
	 * Draw text string
	 * \param x an X coordinate of top left text corner
	 * \param y an Y coordinate of top left text corner
	 * \param scale scale coefficient
	 * \param alpha alpha channel in range [0.0f ... 1.0f]
	 * \param text output string
	 */
	void draw_text(const float x, const float y, const float scale, const float alpha, const char* text);

private:
	/**
	 * Free texture bank
	 */
	void free();

	/**
	 * Show loading message
	 */
	void show_loading();

	/**
	 * Create texture from image
	 * \param img source image
	 * \param mode texture mode
	 * \return loaded texture identifier
	 */
	unsigned int create_texture(const image& img, const int mode) const;

	/**
	 * Make shadow of the tube image
	 * \param img source image
	 * \return tube shadow image
	 */
	image& make_tube_shadow(image& img) const;

	/**
	 * Make shadow of the button image
	 * \param img source image
	 * \return button shadow image
	 */
	image& make_button_shadow(image& img) const;

	/**
	 * Check for OpenGL error
	 * \return true if errors exist
	 */
	bool check_GL_error() const;

private:
	unsigned int _bank[counter]; ///< Texture bank
	unsigned int _loading;       ///< "Loading..." texture
};
