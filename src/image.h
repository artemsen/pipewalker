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

#include "common.h"


class image
{
public:
	image();
	~image();

	/**
	 * Load PNG image from file
	 * \param file_name a file name
	 * \return false if error
	 */
	bool load_PNG(const char* file_name);

	/**
	 * Load XPM image from data buffer
	 * \param data image data
	 * \param str_num string number in image data
	 * \return false if error
	 */
	bool load_XPM(const char* data[], const size_t str_num);

	/**
	 * Image properties: Get image width
	 * \return image width
	 */
	inline size_t width() const       { return _width; }

	/**
	 * Image properties: Get image height
	 * \return image height
	 */
	inline size_t height() const      { return _height; }

	/**
	 * Image properties: Get image data
	 * \return image data (bitmap)
	 */
	inline const void* data() const   { return _data.empty() ? NULL : &_data[0]; }

	/**
	 * Get SDL surface for current image
	 * \return SDL surface pointer
	 */
	SDL_Surface* get_surface();

	/**
	 * Increase canvas by height
	 * \param h new height value
	 */
	void increase_canvas(const size_t h);

	/**
	 * Increase canvas
	 * \param w new width value
	 * \param h new height value
	 */
	void increase_canvas(const size_t w, const size_t h);

	/**
	 * Get sub image
	 * \param x left corner coordinates
	 * \param y top coordinates
	 * \param w sub image width
	 * \param h sub image height
	 * \return sub image
	 */
	image sub_image(const size_t x, const size_t y, const size_t w, const size_t h) const;

	/**
	 * Resize image
	 * \param w new width value
	 * \param h new height value
	 */
	void resize(const size_t w, const size_t h);

	/**
	 * Rotate image
	 * \param angle rotate angle
	 * \return rotated image
	 */
	image rotate(const short angle) const;

	/**
	 * Flip image by vertical
	 * \return flipped image
	 */
	void flip_vertical();

	/**
	 * Blur image by Gaussian algorithm
	 * \param radius blur radius
	 */
	void gaussian_blur(const float radius);

	/**
	 * Add transparency
	 * \param val added alpha value in range [0.0f...1.0f]
	 */
	void add_transparency(const float val);

	/**
	 * Fill image by specified color
	 * \param r red color component in range [0...255]
	 * \param g green color component in range [0...255]
	 * \param b blue color component in range [0...255]
	 */
	void fill_color(const unsigned char r, const unsigned char g, const unsigned char b);

	/**
	 * Get average color of the image
	 * \param r red color component in range [0...255]
	 * \param g green color component in range [0...255]
	 * \param b blue color component in range [0...255]
	 */
	void average_color(unsigned char& r, unsigned char& g, unsigned char& b) const;

private:
	size_t       _width;     ///< Image width
	size_t       _height;    ///< Image height
	SDL_Surface* _sdl_surf;  ///< SDL surface

	typedef vector<unsigned char> img_data;
	img_data _data;          ///< Image data
};
