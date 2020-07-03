// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#include "render.hpp"

#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <png.h>
#include <system_error>
#include <vector>

namespace render {

/** @brief Program Id. */
static GLuint program;
/** @brief Vertex shader Id. */
static GLuint vert_shader;
/** @brief Fragment shader Id. */
static GLuint frag_shader;
/** @brief OpenGL textures. */
static GLuint textures[static_cast<size_t>(Texture::font) + 1];

/** @brief Vertex shader source code. */
static const GLchar* vert_shader_src = R"(#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
out vec2 v_texCoord;
void main()
{
  gl_Position = a_position;
  v_texCoord = a_texCoord;
})";

/** @brief Fragment shader source code. */
static const GLchar* frag_shader_src = R"(#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_texture;
void main()
{
  outColor = texture( s_texture, v_texCoord );
})";

} // namespace render

#ifdef NDEBUG
#define OPENGL_CHECK_ERROR
#else
#define OPENGL_CHECK_ERROR gl_check_error()
static void gl_check_error()
{
    GLenum ec = glGetError();
    if (ec != GL_NO_ERROR) {
        fprintf(stderr, "GL error 0x%x\n", ec);
        abort();
    }
}
#endif // NDEBUG

ViewPort::ViewPort()
{
    glGetIntegerv(GL_VIEWPORT, vp);
}

ViewPort::ViewPort(size_t cols, size_t rows)
{
    glGetIntegerv(GL_VIEWPORT, vp);

    GLint scr_w = vp[2];
    GLint scr_h = vp[3];
    const float cell_width = static_cast<float>(scr_w) / static_cast<float>(cols);
    const float cell_height = static_cast<float>(scr_h) / static_cast<float>(rows);
    if (cell_width > cell_height) {
        scr_w = cell_height * cols;
    } else {
        scr_h = cell_width * rows;
    }
    const GLint side_size = std::max(scr_w, scr_h);

    vp[0] = (vp[2] - side_size) / 2;
    vp[1] = (vp[3] - side_size) / 2;
    vp[2] = side_size;
    vp[3] = side_size;
}

ViewPort::~ViewPort()
{
    if (bk[2] && bk[3]) {
        glViewport(bk[0], bk[1], bk[2], bk[3]);
    }
}

void ViewPort::apply()
{
    glGetIntegerv(GL_VIEWPORT, bk);
    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

Coordinates ViewPort::to_world(size_t x, size_t y) const
{
    const GLint scr_width = vp[0] * 2 + vp[2];
    const GLint scr_height = vp[1] * 2 + vp[3];
    const float pixel_size = 2.0f / vp[2];
    return Coordinates {
        (static_cast<float>(x) - scr_width / 2) * pixel_size,
        (static_cast<float>(y) - scr_height / 2) * pixel_size,
    };
}

const GLushort Painter::indices[6] = { 0, 1, 2, 0, 2, 3 };

Painter::Painter(const Coordinates& top_left, const Coordinates& bottom_right)
{
    vertices[0 * vc_num + 0] = top_left.x;
    vertices[0 * vc_num + 1] = top_left.y;
    vertices[1 * vc_num + 0] = top_left.x;
    vertices[1 * vc_num + 1] = bottom_right.y;
    vertices[2 * vc_num + 0] = bottom_right.x;
    vertices[2 * vc_num + 1] = bottom_right.y;
    vertices[3 * vc_num + 0] = bottom_right.x;
    vertices[3 * vc_num + 1] = top_left.y;
}

Painter::Painter(const Coordinates& tl, float width, float height)
{
    const Coordinates br { tl.x + width, tl.y - height };
    vertices[0 * vc_num + 0] = tl.x;
    vertices[0 * vc_num + 1] = tl.y;
    vertices[1 * vc_num + 0] = tl.x;
    vertices[1 * vc_num + 1] = br.y;
    vertices[2 * vc_num + 0] = br.x;
    vertices[2 * vc_num + 1] = br.y;
    vertices[3 * vc_num + 0] = br.x;
    vertices[3 * vc_num + 1] = tl.y;
}

Painter& Painter::move(float x, float y)
{
    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += vc_num) {
        vertices[i + 0] += x;
        vertices[i + 1] += y;
    }
    return *this;
}

Painter& Painter::rotate(float angle)
{
    const float rads = -angle * M_PI / 180.0f;
    const float cs = cos(rads);
    const float sn = sin(rads);
    const float delta_x = vertices[0] + (vertices[2 * vc_num] - vertices[0]) / 2.0f;
    const float delta_y = vertices[1] + (vertices[2 * vc_num + 1] - vertices[1]) / 2.0f;
    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += vc_num) {
        const GLfloat x = vertices[i + 0] - delta_x;
        const GLfloat y = vertices[i + 1] - delta_y;
        vertices[i + 0] = delta_x + x * cs - y * sn;
        vertices[i + 1] = delta_y + x * sn + y * cs;
    }
    return *this;
}

Painter& Painter::scale(float s)
{
    const float delta_x = vertices[0] + (vertices[2 * vc_num] - vertices[0]) / 2.0f;
    const float delta_y = vertices[1] + (vertices[2 * vc_num + 1] - vertices[1]) / 2.0f;
    for (size_t i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i += vc_num) {
        const GLfloat x = vertices[i + 0] - delta_x;
        const GLfloat y = vertices[i + 1] - delta_y;
        vertices[i + 0] = delta_x + x * s;
        vertices[i + 1] = delta_y + y * s;
    }
    return *this;
}

Painter& Painter::repeat(float x, float y)
{
    for (size_t i = 0; i < sizeof(texture) / sizeof(texture[0]); i += tc_num) {
        texture[i + 0] *= x;
        texture[i + 1] *= y;
    }
    return *this;
}

void Painter::draw(Texture tex) const
{
    // load the vertex position and texture coordinate
    glVertexAttribPointer(0, Painter::vc_num, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(1, Painter::tc_num, GL_FLOAT, GL_FALSE, 0, texture);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render::textures[static_cast<size_t>(tex)]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Painter::indices);
}

void Painter::draw(const char* text)
{
    assert(text);

    // Font texture parameters: number of chars in a row and column
    constexpr size_t ft_rows = 16;
    constexpr size_t ft_cols = 8;
    // Font texture parameters: size of a char
    constexpr float ft_width = 1.0f / ft_rows;
    constexpr float ft_height = 1.0f / ft_cols;

    // we need to fit into rectangle
    const float text_width = vertices[2 * vc_num + 0] - vertices[0 * vc_num + 0];
    const float char_width = text_width / strlen(text);
    vertices[2 * vc_num + 0] = vertices[3 * vc_num + 0] = vertices[0 * vc_num + 0] + char_width + 0.05f;

    while (*text) {
        assert(*text >= ' ' && *text <= '~');

        // texture coordinates for current char
        const size_t chr = *text - ' ';
        const size_t col = chr % ft_rows;
        const size_t row = chr / ft_rows;
        const float tex_x = ft_width * col;
        const float tex_y = ft_height * row;
        const Coordinates top_left { tex_x, tex_y };
        const Coordinates bottom_right { tex_x + ft_width, tex_y + ft_height };
        texture[0 * tc_num + 0] = top_left.x;
        texture[0 * tc_num + 1] = top_left.y;
        texture[1 * tc_num + 0] = top_left.x;
        texture[1 * tc_num + 1] = bottom_right.y;
        texture[2 * tc_num + 0] = bottom_right.x;
        texture[2 * tc_num + 1] = bottom_right.y;
        texture[3 * tc_num + 0] = bottom_right.x;
        texture[3 * tc_num + 1] = top_left.y;

        draw(Texture::font);
        move(char_width, 0.0f);
        ++text;
    }
}

/** @brief Image loader. */
struct Image {
    size_t width = 0;           ///< Image width
    size_t height = 0;          ///< Image height
    std::vector<uint32_t> data; ///< Image data

    Image() = default;

    /**
     * @brief Constructor: load PNG image from file.
     *
     * @param[in] file path to the PNG file
     *
     * @throw std::runtime_error in case of errors
     */
    Image(const char* file)
    {
        png_structp png = nullptr;
        png_infop info = nullptr;

        try {
            std::unique_ptr<FILE, int (*)(FILE*)> fd(fopen(file, "rb"), fclose);
            if (!fd) {
                throw std::system_error(errno, std::system_category());
            }
            png = png_create_read_struct(
                PNG_LIBPNG_VER_STRING, nullptr, [](png_structp, png_const_charp err) { throw std::runtime_error(err); },
                [](png_structp, png_const_charp) {});

            if (!png) {
                throw std::runtime_error("Unable to create libpng read object");
            }
            info = png_create_info_struct(png);
            if (!info) {
                throw std::runtime_error("Unable to create libpng info object");
            }

            png_init_io(png, fd.get());
            png_read_info(png, info);
            png_set_bgr(png);

            width = png_get_image_width(png, info);
            height = png_get_image_height(png, info);

            if (png_get_color_type(png, info) != PNG_COLOR_TYPE_RGBA) {
                throw std::runtime_error("Invalid image: no alpha channel");
            }

            data.resize(height * width);
            std::vector<png_bytep> rows(height);
            for (size_t y = 0; y < height; ++y) {
                rows[y] = reinterpret_cast<png_bytep>(&data[y * width]);
            }
            png_read_image(png, rows.data());

            png_destroy_read_struct(&png, &info, nullptr);

        } catch (const std::exception&) {
            if (png) {
                png_destroy_read_struct(&png, info ? &info : nullptr, nullptr);
            }
            width = 0;
            height = 0;
            data.clear();
            throw;
        }

        // todo
        for (size_t i = 0; i < data.size(); ++i) {
            // r <-> b
            uint32_t n = data[i];
            data[i] = (n & 0xff00ff00) | ((n & 0x00ff0000) >> 16) | ((n & 0x000000ff) << 16);
        }
    }

    Image tile(size_t x, size_t y, size_t w, size_t h) const
    {
        const size_t cell_size = 32;

        // calculate cell position and size in pixels
        const size_t offset_x = x * cell_size;
        const size_t offset_y = y * cell_size;
        const size_t sz_width = w * cell_size;
        const size_t sz_height = h * cell_size;
        assert(offset_x + sz_width <= width);
        assert(offset_y + sz_height <= height);

        // get sub image
        Image img;
        img.width = sz_width;
        img.height = sz_height;
        img.data.resize(img.width * img.height);
        for (size_t y = 0; y < img.height; ++y) {
            const auto it = data.begin() + offset_x + (offset_y + y) * width;
            std::copy(it, it + img.width, img.data.begin() + y * img.width);
        }

        // image size must be power of 2
        assert(!(img.width & (img.width - 1))); // width is always power of 2
        if (img.height & (img.height - 1)) {
            // append new lines, up to width size
            assert(img.height < img.width);
            assert(img.height < 256);
            const size_t add_lines = 256 - img.height;
            img.data.insert(img.data.end(), add_lines * img.width, 0);
            img.height += add_lines;
        }

        return img;
    }
};

/** @brief Texture position inside the texture file. */
struct TextureMap {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    Texture type;
};

/** @brief Texture file scheme. */
static const TextureMap tex_map[] = {
    // row 1
    { 0, 0, 2, 2, Texture::bkg_env },
    { 2, 0, 2, 2, Texture::bkg_cell },
    { 4, 0, 2, 2, Texture::sender },
    { 6, 0, 2, 2, Texture::receiver_psv },
    { 8, 0, 2, 2, Texture::receiver_act },
    { 10, 0, 2, 2, Texture::lock },
    { 12, 0, 1, 1, Texture::explosion },
    // row 2 (pipes)
    { 0, 2, 2, 2, Texture::pipe_hlf_psv },
    { 2, 2, 2, 2, Texture::pipe_hlf_act },
    { 4, 2, 2, 2, Texture::pipe_str_psv },
    { 6, 2, 2, 2, Texture::pipe_str_act },
    { 8, 2, 2, 2, Texture::pipe_bnt_psv },
    { 10, 2, 2, 2, Texture::pipe_bnt_act },
    { 12, 2, 2, 2, Texture::pipe_frk_psv },
    { 14, 2, 2, 2, Texture::pipe_frk_act },
    // row 3 (buttons)
    { 0, 4, 2, 2, Texture::btn_prev },
    { 2, 4, 2, 2, Texture::btn_next },
    { 4, 4, 2, 2, Texture::btn_ok },
    { 6, 4, 2, 2, Texture::btn_cancel },
    { 8, 4, 2, 2, Texture::btn_reset },
    { 10, 4, 2, 2, Texture::btn_sett },
    { 12, 4, 2, 2, Texture::btn_cb_on },
    { 14, 4, 2, 2, Texture::btn_cb_off },
    // row 4
    { 0, 6, 16, 2, Texture::title },
    // font
    { 0, 8, 16, 6, Texture::font },
};

namespace render {

/**
 * @brief Load shader.
 *
 * @param[in] src source code
 * @param[in] type shader type
 *
 * @throw std::runtime_error in case of errors
 *
 * @return shader id
 */
static GLuint load_shader(const GLchar* src, GLenum type)
{
    GLuint shader = glCreateShader(type);
    assert(shader);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // check the compile status
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char err[128];
        glGetShaderInfoLog(shader, sizeof(err), nullptr, err);
        fprintf(stderr, "Error compiling shader: %s\n", err);
        abort();
    }

    return shader;
}

void initialize()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // create the program object
    program = glCreateProgram();
    if (!program) {
        throw std::runtime_error("Unable to create OpenGL program");
    }
    // Load the vertex/fragment shaders
    vert_shader = load_shader(vert_shader_src, GL_VERTEX_SHADER);
    glAttachShader(program, vert_shader);
    frag_shader = load_shader(frag_shader_src, GL_FRAGMENT_SHADER);
    glAttachShader(program, frag_shader);

    // link the program
    glLinkProgram(program);
    GLint link_status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
        char err[128];
        glGetProgramInfoLog(program, sizeof(err), nullptr, err);
        throw std::runtime_error(std::string("Unable to link OpenGL program: ") + err);
    }

    glUseProgram(program);

    // set the sampler texture unit to 0
    GLint loc = glGetUniformLocation(program, "s_texture");
    assert(loc >= 0);
    glUniform1i(loc, 0);

    OPENGL_CHECK_ERROR;
}

void finalize()
{
    if (program) {
        glDeleteProgram(program);
    }
    if (vert_shader) {
        glDeleteShader(vert_shader);
    }
    if (frag_shader) {
        glDeleteShader(frag_shader);
    }
    // todo: delete textures
}

void load_textures(const char* file)
{
    // todo: delete old textures

    Image img(file);
    if (img.width != 512 || img.height != 448) {
        throw std::runtime_error("Invalid image size: nexpected 512x448");
    }

    for (const auto& tx : tex_map) {
        const Image tile = img.tile(tx.x, tx.y, tx.width, tx.height);

        const GLint wrap = tx.type == Texture::bkg_env || tx.type == Texture::bkg_cell ? GL_REPEAT : GL_CLAMP_TO_EDGE;

        // create OpenGL texture
        GLuint id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(tile.width), static_cast<GLsizei>(tile.height), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, tile.data.data());

        textures[static_cast<size_t>(tx.type)] = id;
    }

    OPENGL_CHECK_ERROR;
}

void resize(size_t width, size_t height)
{
    glViewport(0, 0, width, height);
}

void clear()
{
    ViewPort vp;
    const size_t max_sz = std::max(vp.vp[2], vp.vp[3]);
    vp.vp[2] = max_sz;
    vp.vp[3] = max_sz;
    vp.apply();

    const float repeat = static_cast<float>(max_sz) / 64.0f /* size of the tile */;
    Painter().scale(2.0f).repeat(repeat, repeat).draw(Texture::bkg_env);
}

} // namespace render
