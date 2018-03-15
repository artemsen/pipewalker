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

#include "image.h"
#ifdef WIN32
#include <GdiPlus.h>
#else
#include <png.h>
#endif // WIN32

#define BPP	4	//Byte per pixel


image::image()
:	_width(0), _height(0),
	_sdl_surf(NULL)
{
}


image::~image()
{
	if (_sdl_surf)
		SDL_FreeSurface(_sdl_surf);
}


bool image::load_PNG(const char* file_name)
{
	assert(file_name && *file_name);

	bool loaded = false;

	_data.clear();

#ifdef WIN32
	ULONG_PTR gdiplus_token;
	Gdiplus::GdiplusStartupInput gdiplus_si;
	Gdiplus::GdiplusStartup(&gdiplus_token, &gdiplus_si, NULL);
	Gdiplus::Bitmap* img = NULL;

	try {
		//Convert file name from ansi to wide
		wstring wide_file_name(strlen(file_name) + 1, 0);
		MultiByteToWideChar(CP_ACP, 0, file_name, -1, &wide_file_name[0], static_cast<int>(wide_file_name.size()));

		//Open image
		img = Gdiplus::Bitmap::FromFile(wide_file_name.c_str());
		if (!img) {
			fprintf(stderr, "Failed to load file %s\n", file_name);
			throw 0;
		}
		if (img->GetPixelFormat() != PixelFormat32bppARGB) {
			fprintf(stderr, "File %s has no alpha channel\n", file_name);
			throw 0;	//Unsupported color
		}

		_width = img->GetWidth();
		_height = img->GetHeight();

		Gdiplus::BitmapData bmp_data;
		Gdiplus::Rect rect(0, 0, static_cast<INT>(_width), static_cast<INT>(_height));
		if (img->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmp_data) != Gdiplus::Ok) {
			fprintf(stderr, "Error reading data from image file %s\n", file_name);
			throw 0;
		}

		//Copy source data
		_data.resize(_width * _height * BPP);
		for (size_t y = 0; y < _height; ++y) {
			const size_t dts_row = y * _width * BPP;
			const unsigned char* src_row = static_cast<const unsigned char*>(bmp_data.Scan0) + dts_row;
			for (size_t x = 0; x < _width; ++x) {
				_data[dts_row + x * BPP + 0] = src_row[x * BPP + 2];
				_data[dts_row + x * BPP + 1] = src_row[x * BPP + 1];
				_data[dts_row + x * BPP + 2] = src_row[x * BPP + 0];
				_data[dts_row + x * BPP + 3] = src_row[x * BPP + 3];
			}
		}

		img->UnlockBits(&bmp_data);
		loaded = true;
	}
	catch (...) {
	}

	delete img;
	Gdiplus::GdiplusShutdown(gdiplus_token);
#else

	FILE* img_file = NULL;
	png_structp png_str = NULL;
	png_infop png_info = NULL;

	try {
		img_file = fopen(file_name, "rb");
		if (!img_file) {
			fprintf(stderr, "Failed to load file %s\n", file_name);
			throw 0;
		}
		png_byte img_header[8];
		if (fread(img_header, 1, sizeof(img_header), img_file) != sizeof(img_header) || png_sig_cmp(img_header, 0, sizeof(img_header))) {
			fprintf(stderr, "File %s is not PNG format\n", file_name);
			throw 0;	//Not PNG format
		}
		png_str = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_str)
			throw 0;
		png_info = png_create_info_struct(png_str);
		if (!png_info)
			throw 0;
		if (setjmp(png_jmpbuf(png_str)))
			throw 0;	//Error during init_io
		png_init_io(png_str, img_file);
		png_set_sig_bytes(png_str, sizeof(img_header));
		png_read_info(png_str, png_info);

		_width = png_get_image_width(png_str, png_info);
		_height = png_get_image_height(png_str, png_info);

		const png_byte color_type = png_get_color_type(png_str, png_info);
		if (color_type != PNG_COLOR_TYPE_RGBA) {
			fprintf(stderr, "File %s has no alpha channel\n", file_name);
			throw 0;	//Unsupported color
		}

		png_set_interlace_handling(png_str);
		png_read_update_info(png_str, png_info);

		if (setjmp(png_jmpbuf(png_str)))
			throw 0;	//Error during read_image

		vector<png_bytep> rows_ptr(_height);
		vector< vector<png_byte> > rows_dat(_height);
		const size_t row_sz = png_get_rowbytes(png_str, png_info);
		for (size_t i = 0; i < _height; ++i) {
			rows_dat[i].resize(row_sz);
			rows_ptr[i] = &rows_dat[i].front();
		}
		png_read_image(png_str, &rows_ptr.front());

		_data.reserve(_height * row_sz);
		for (size_t i = 0; i < _height; ++i)
			copy(rows_ptr[i], rows_ptr[i] + row_sz, back_inserter(_data));

		loaded = true;
	}
	catch (...) {
	}

	png_destroy_read_struct(png_str ? &png_str : NULL, png_info ? &png_info : NULL, NULL);
	if (img_file)
		fclose(img_file);

#endif // WIN32

	assert(_data.size() == _width * _height * BPP);

	return loaded;
}


bool image::load_XPM(const char* data[], const size_t str_num)
{
	assert(data);

	_data.clear();

	//Read image properties
	int color_map_size = 0, xpm_bpp = 0, xmp_width = 0, xpm_height = 0;
	if (sscanf(data[0], "%i %i %i %i", &xmp_width, &xpm_height, &color_map_size, &xpm_bpp) != 4)
		return false;
	_width = static_cast<size_t>(xmp_width);
	_height = static_cast<size_t>(xpm_height);

	//Read the color map
	map< string, vector<unsigned char> > color_map;
	for (int i = 1; i <= color_map_size; ++i) {
		if (i >= static_cast<int>(str_num))
			return false;
		const string index(data[i], data[i] + xpm_bpp);
		vector<unsigned char> color(4);
		if (strcmp(data[i] + xpm_bpp + 3, "None") != 0) {
			unsigned int color_ref = 0;
			sscanf(data[i] + xpm_bpp + 4, "%x", &color_ref);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
			color[0] = static_cast<unsigned char>((color_ref & 0x00ff0000) >> 16);
			color[1] = static_cast<unsigned char>((color_ref & 0x0000ff00) >> 8);
			color[2] = static_cast<unsigned char>((color_ref & 0x000000ff) >> 0);
#else
			color[0] = static_cast<unsigned char>((color_ref & 0xff000000) << 0);
			color[1] = static_cast<unsigned char>((color_ref & 0x00ff0000) << 8);
			color[2] = static_cast<unsigned char>((color_ref & 0x0000ff00) << 16);
#endif	//SDL_BYTEORDER == SDL_BIG_ENDIAN
			color[3] = 0xff;	//Non transparent
		}
		color_map.insert(make_pair(index, color));
	}

	//Read image data
	_data.reserve(_width * _height * BPP);
	for (size_t i = 0; i < _height; ++i) {
		if (i + color_map_size + 1 >= str_num)
			return false;
		const char* img_data = data[i + color_map_size + 1];
		for (size_t j = 0; j < _width; ++j) {
			const string color_index(img_data + j * xpm_bpp, img_data + j * xpm_bpp + xpm_bpp);
			map< string, vector<unsigned char> >::const_iterator it = color_map.find(color_index);
			if (it == color_map.end())
				return false;
			copy(it->second.begin(), it->second.end(), back_inserter(_data));
		}
	}

	return (_data.size() == _width * _height * BPP);
}


SDL_Surface* image::get_surface()
{
	if (_sdl_surf)
		return _sdl_surf;

	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif	//SDL_BYTEORDER == SDL_BIG_ENDIAN

	_sdl_surf = SDL_CreateRGBSurfaceFrom(&_data.front(), static_cast<int>(_width), static_cast<int>(_height), 32, static_cast<int>(_width * BPP), rmask, gmask, bmask, amask);

	return _sdl_surf;
}


void image::increase_canvas(const size_t h)
{
	assert(h > _height);
	_data.insert(_data.begin(), (h - _height) * _width * BPP, 0);
	_height = h;
}


void image::increase_canvas(const size_t w, const size_t h)
{
	assert(w > _width);
	assert(h > _height);

	image img;
	img._width  = w;
	img._height = h;
	img._data.resize(img._width * img._height * BPP);

	const size_t new_pos_x = (w - _width) / 2;
	const size_t new_pos_y = (h - _height) / 2;

	//Copy current image to center of new canvas
	for (size_t y = 0; y < _height; ++y) {
		memcpy(
			&img._data[(new_pos_y + y) * img._width * BPP + new_pos_x * BPP],
			&_data[y * _width * BPP],
			_width * BPP);
	}

	*this = img;
}


image image::sub_image(const size_t x, const size_t y, const size_t w, const size_t h) const
{
	assert(!_data.empty());

	if (x > _width || x + w > _width || y > _height || y + h > _height)
		return *this;	//Sub image coordinates out of range

	image img;
	img._width = w;
	img._height = h;
	img._data.reserve(h * w * BPP);

	for (size_t i = 0; i < h; ++i) {
		const img_data::const_iterator it = _data.begin() + x * BPP + (i + y) * _width * BPP;
		copy(it, it + w * BPP, back_inserter(img._data));
	}

	return img;
}


void image::resize(const size_t w, const size_t h)
{
	image img;
	img._width  = w;
	img._height = h;
	img._data.reserve(img._width * img._height * BPP);

	const float factor_x = static_cast<float>(img._width) / static_cast<float>(_width);
	const float factor_y = static_cast<float>(img._height) / static_cast<float>(_height);

	for (size_t y = 0; y < img._height; ++y) {
		const size_t row_src = static_cast<size_t>(static_cast<float>(y) / factor_y) * _width * BPP;
		for (size_t x = 0; x < img._width; ++x) {
			const unsigned char* data = &_data[row_src + static_cast<size_t>(static_cast<float>(x) / factor_x) * BPP];
			copy(data, data + BPP, back_inserter(img._data));
		}
	}

	*this = img;
}


image image::rotate(const short angle) const
{
	short degree = angle;
	while (degree >= 360)
		degree -= 360;
	while (degree < 0)
		degree += 360;

	image img;

	if (degree == 90) {
		img._width = _height;
		img._height = _width;
		img._data.resize(_data.size());
		for (size_t y = 0; y < _height; ++y) {
			for (size_t x = 0; x < _width; ++x) {
				memcpy(
					&img._data[(img._width - y - 1) * BPP + x * img._width * BPP],
					&_data[y * _width * BPP + x * BPP], BPP);
			}
		}
	}
	else if (degree == 270 /* -90 */) {
		img._width = _height;
		img._height = _width;
		img._data.resize(_data.size());
		for (size_t y = 0; y < _height; ++y) {
			for (size_t x = 0; x < _width; ++x) {
				memcpy(
					&img._data[(img._height - x - 1) * img._width * BPP + y * BPP],
					&_data[y * _width * BPP + x * BPP], BPP);
			}
		}
	}
	else if (degree == 180) {
		img._width = _width;
		img._height = _height;
		img._data.resize(_data.size());
		for (size_t y = 0; y < img._height; ++y) {
			const size_t row_src = (_height - y - 1) * _width * BPP;
			const size_t col_src = y * _width * BPP;
			for (size_t x = 0; x < img._width; ++x) {
				const unsigned char* src_data = &_data[col_src + x * BPP];
				memcpy(&img._data[row_src + (img._width - x - 1) * BPP], src_data, BPP);
			}
		}
	}
	else {
		const float radians = 3.14159f / 180.0f * degree;
		const float sinma = sin(radians);
		const float cosma = cos(radians);
		img._width = _width;
		img._height = _height;
		img._data.resize(img._width * img._height * BPP);
		const int half_width = static_cast<int>(_width / 2);
		const int half_height = static_cast<int>(_height / 2);
		for (size_t y = 0; y < img._height; ++y) {
			const int yt = static_cast<int>(y) - half_height;
			for (size_t x = 0; x < img._width; ++x) {
				const int xt = static_cast<int>(x) - half_width;
				const int xs = static_cast<int>(cosma * xt - sinma * yt) + half_width;
				const int ys = static_cast<int>(sinma * xt + cosma * yt) + half_height;
				if (xs >= 0 && xs < static_cast<int>(_width) && ys >= 0 && ys < static_cast<int>(_height))
					memcpy(&img._data[y * img._width * BPP + x * BPP], &_data[ys * _width * BPP + xs * BPP], BPP);
			}
		}
	}

	return img;
}


void image::flip_vertical()
{
	assert(!_data.empty());

	const size_t ln_size = _width * BPP;
	vector<unsigned char> swap_buffer(ln_size);
	unsigned char* swap_buff = &swap_buffer.front();
	const size_t ln_count = _height / 2;
	for (size_t y = 0; y < ln_count; ++y) {
		unsigned char* src = &_data[y * _width * BPP];
		unsigned char* dst = &_data[(_height - y - 1) * _width * BPP];
		memcpy(swap_buff, src, ln_size);
		memcpy(src, dst, ln_size);
		memcpy(dst, swap_buff, ln_size);
	}
}


void image::gaussian_blur(const float radius)
{
	assert(!_data.empty());

	image tmp;
	tmp._width  = _width;
	tmp._height = _height;
	tmp._data.resize(tmp._width * tmp._height * BPP);

	const float sigma2 = radius * radius;
	const int size = 5;	//Good approximation of filter

	float sum = 0;
	float pixel[4];
	for (int y = 0; y < static_cast<int>(_height); ++y) {
		for (int x = 0; x < static_cast<int>(_width); ++x) {
			sum = pixel[0] = pixel[1] = pixel[2] = pixel[3] = 0.0f;
			//Accumulate colors
			for (int i = max(0, x - size); i <= min(static_cast<int>(_width) - 1, x + size); ++i) {
				const float factor = exp(-(i - x) * (i - x) / (2.0f * sigma2));
				sum += factor;
				const unsigned char* color = &_data[y * _width * BPP + i * BPP];
				pixel[0] += factor * color[0];
				pixel[1] += factor * color[1];
				pixel[2] += factor * color[2];
				pixel[3] += factor * color[3];
			};
			unsigned char* color = &tmp._data[y * _width * BPP + x * BPP];
			color[0] = static_cast<unsigned char>(pixel[0] / sum);
			color[1] = static_cast<unsigned char>(pixel[1] / sum);
			color[2] = static_cast<unsigned char>(pixel[2] / sum);
			color[3] = static_cast<unsigned char>(pixel[3] / sum);
		}
	}

	for (int y = 0; y < static_cast<int>(_height); ++y) {
		for (int x = 0; x < static_cast<int>(_width); ++x) {
			sum = pixel[0] = pixel[1] = pixel[2] = pixel[3] = 0.0f;
			//Accumulate colors
			for(int i = max(0, y - size); i <= min(static_cast<int>(_height) - 1, y + size); ++i) {
				const float factor = exp(-(i - y) * (i - y) / (2.0f * sigma2));
				sum += factor;
				const unsigned char* color = &tmp._data[i * _width * BPP + x * BPP];
				pixel[0] += factor * color[0];
				pixel[1] += factor * color[1];
				pixel[2] += factor * color[2];
				pixel[3] += factor * color[3];
			};
			unsigned char* color = &_data[y * _width * BPP + x * BPP];
			color[0] = static_cast<unsigned char>(pixel[0] / sum);
			color[1] = static_cast<unsigned char>(pixel[1] / sum);
			color[2] = static_cast<unsigned char>(pixel[2] / sum);
			color[3] = static_cast<unsigned char>(pixel[3] / sum);
		}
	}
}


void image::add_transparency(const float val)
{
	assert(!_data.empty());
	assert(val > 0.0f && val < 1.01f);

	const size_t pix_num = _width * _height;
	for (size_t i = 0; i < pix_num; ++i) {
		unsigned char& a = _data[i * BPP + 3];
		a = static_cast<unsigned char>(val * a);
	}
}


void image::fill_color(const unsigned char r, const unsigned char g, const unsigned char b)
{
	for (size_t y = 0; y < _height; ++y) {
		const size_t row_src = y * _width * BPP;
		for (size_t x = 0; x < _width; ++x) {
			_data[row_src + x * BPP + 0] = r;
			_data[row_src + x * BPP + 1] = g;
			_data[row_src + x * BPP + 2] = b;
		}
	}
}


void image::average_color(unsigned char& r, unsigned char& g, unsigned char& b) const
{
	size_t sum_r = 0, sum_g = 0, sum_b = 0;
	for (size_t y = 0; y < _height; ++y) {
		const size_t row_src = y * _width * BPP;
		for (size_t x = 0; x < _width; ++x) {
			sum_r += _data[row_src + x * BPP + 0];
			sum_g += _data[row_src + x * BPP + 1];
			sum_b += _data[row_src + x * BPP + 2];
		}
	}
	const size_t max_sum_color = max(sum_r, max(sum_g, sum_b));
	r = static_cast<unsigned char>((static_cast<float>(sum_r) / max_sum_color) * 255.0f);
	g = static_cast<unsigned char>((static_cast<float>(sum_g) / max_sum_color) * 255.0f);
	b = static_cast<unsigned char>((static_cast<float>(sum_b) / max_sum_color) * 255.0f);
}
