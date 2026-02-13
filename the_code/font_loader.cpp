//
// Created by klaas on 1/31/2026.
//
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_treutype.h"
#include "vector"
#include "string"
#include "fstream"
#include <unordered_map>
#include <array>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <pybind11/stl.h>

using namespace std;
namespace py = pybind11;

struct Glyph {
    int advance;
    int lsb;
    int x0;
    int x1;
    int y0;
    int y1;
    stbtt_vertex* vertices;
    int num_vertices;
};

class Font {
    stbtt_fontinfo font;                         // parsed font
    vector<unsigned char> fontData;         // raw file bytes
    unordered_map<int, Glyph> glyphs;      // Unicode → Glyph

public:
    explicit Font(const string& filePath) : font{} {
        ifstream file(filePath, ios::binary);
        fontData = vector<unsigned char>(istreambuf_iterator<char>(file), {});

        // initialize stb_truetype
        stbtt_InitFont(&font, fontData.data(), 0);

    }

    ~Font(){
        for (auto& [_, g] : glyphs){
            stbtt_FreeShape(&font, g.vertices);
        }
    }

    py::array_t<uint8_t> get_raster_from_glyph(float pixelHeight, int unicode) {
        float scale = stbtt_ScaleForPixelHeight(&font, pixelHeight);
        int glyph_index = stbtt_FindGlyphIndex(&font, unicode);
        if (glyph_index == 0) {
            return py::array_t<uint8_t>({0,0}, nullptr); // empty bitmap if glyph not found
        }

        int w, h, xoff, yoff;
        unsigned char* bitmap = stbtt_GetGlyphBitmap(&font, scale, scale, glyph_index, &w, &h, &xoff, &yoff);

        return wrap_uc_ptr(bitmap, w, h);
    }
    std::tuple<int,int,int,int,int> get_render_info(float pixel_height, int unicode) {

        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

        float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);

        const Glyph &glyph = getGlyph(unicode);

        int scaledAscent  = static_cast<int>(roundf(ascent * scale));
        int scaledDescent = static_cast<int>(roundf(descent * scale));
        int scaledLineGap = static_cast<int>(roundf(lineGap * scale));

        int lsb     = static_cast<int>(roundf(glyph.lsb * scale));
        int advance = static_cast<int>(roundf(glyph.advance * scale));

        return { advance, lsb,
                 scaledAscent,
                 scaledDescent,
                 scaledLineGap };
    }
private:
    const Glyph& getGlyph(int unicode) {
        if (glyphs.contains(unicode)) {
            return glyphs.at(unicode);
        }
        int glyph_index = stbtt_FindGlyphIndex(&font, unicode);
        if (glyph_index == 0) {
            std::cout << "unable to find glyph index " << unicode << std::endl;
            glyph_index = 0;
            static Glyph emptyGlyph{0,0,0,0,0,0,nullptr,0};
            return emptyGlyph;
        }
        int advance, lsb;
        stbtt_GetGlyphHMetrics(&font, glyph_index, &advance, &lsb);
        int x0, y0, x1, y1;
        stbtt_GetGlyphBox(&font, glyph_index, &x0, &y0, &x1, &y1);
        stbtt_vertex* vertices;
        int num_verts = stbtt_GetGlyphShape(&font, glyph_index, &vertices);
        Glyph glyph;
        glyph.advance = advance;
        glyph.lsb = lsb;
        glyph.x0 = x0;
        glyph.y0 = y0;
        glyph.x1 = x1;
        glyph.y1 = y1;
        glyph.num_vertices = num_verts;
        glyph.vertices = vertices;
        glyphs[unicode] = glyph;
        return glyphs[unicode];
    }

    static py::array_t<uint8_t> wrap_uc_ptr(
    unsigned char* data,
    int width,
    int height
) {
        auto capsule = py::capsule(data, [](void* p) {
            free(p);   // or delete[] / custom free
        });

        return py::array_t<uint8_t>(
            { height, width },   // shape
            { width, 1 },        // strides
            data,
            capsule
        );
    }
};

PYBIND11_MODULE(font_holder, m) {
    py::class_<Font>(m, "Font")
    .def(py::init<const std::string&>())
    .def("get_raster_from_glyph", &Font::get_raster_from_glyph)
    .def("get_render_info", &Font::get_render_info);
}