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
        // load font into memory
        ifstream file(filePath, ios::binary);
        fontData = vector<unsigned char>(istreambuf_iterator<char>(file), {});

        // initialize stb_truetype
        stbtt_InitFont(&font, fontData.data(), 0);

    }

    py::array_t<uint8_t> get_raster_from_glyph(float pixelHeight, int unicode) {
        float scale = stbtt_ScaleForPixelHeight(&font, pixelHeight);
        Glyph glyph = getGlyph(unicode);
        array<int, 4> scaled_points = {static_cast<int>(ceil(glyph.x0 * scale)),static_cast<int>(ceil(glyph.x1 * scale)),static_cast<int>(ceil(glyph.y0 * scale)),static_cast<int>(ceil(glyph.y1 * scale))};
        int w = scaled_points[1] - scaled_points[0];
        int h = scaled_points[3] - scaled_points[2];
        stbtt__bitmap bmp{};
        bmp.w = w;
        bmp.h = h;
        bmp.stride = w;
        bmp.pixels = (unsigned char*)malloc(w * h);

        memset(bmp.pixels, 0, w * h);
        stbtt_vertex* verts = glyph.vertices;
        int num_vertices = glyph.num_vertices;

        stbtt_Rasterize(&bmp,0.35f, verts,num_vertices,scale,scale,0, 0,scaled_points[0], scaled_points[2],1,nullptr);
        py::array_t<uint8_t> rasterized_letter = wrap_uc_ptr(bmp.pixels,bmp.w,bmp.h);
        return rasterized_letter;
    }
private:
    const Glyph& getGlyph(int unicode) {
        if (glyphs.contains(unicode)) {
            return glyphs.at(unicode);
        }
        int glyph_index = stbtt_FindGlyphIndex(&font, unicode);
        if (glyph_index == 0) {
            glyph_index = 0;
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
    .def("get_raster_from_glyph", &Font::get_raster_from_glyph);
}