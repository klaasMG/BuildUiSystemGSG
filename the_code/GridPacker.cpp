//
// Created by klaas on 2/16/2026.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "rectangle_packing_utils/RectBinPack/grid_packer.h"

namespace py = pybind11;

struct PlacedRect{
    uint32_t id;
    uint16_t pos_x;
    uint16_t pos_y;
};

class TextPacker {
    grid_data grid;
public:
    TextPacker()
        : grid(8192, 8192, 32, 16){}

    std::pair<bool,PlacedRect> add(uint32_t id, uint32_t pixel_size_x, uint32_t pixel_size_y){
        uint16_t placed_x;
        uint16_t placed_y;
        bool placed = grid.add(id, pixel_size_x, pixel_size_y, placed_x, placed_y);
        PlacedRect placed_rect = {id,placed_x,placed_y};
        std::pair<bool,PlacedRect> return_pair;
        return_pair.first = placed;
        return_pair.second = placed_rect;
        return return_pair;
    }

    bool remove(uint32_t id){
        bool removed = grid.remove(id);
        return removed;
    }

};

PYBIND11_MODULE(GridPacker, m) {
    py::class_<PlacedRect>(m, "PlacedRect")
        .def_readonly("id", &PlacedRect::id)
        .def_readonly("pos_x", &PlacedRect::pos_x)
        .def_readonly("pos_y", &PlacedRect::pos_y);

    py::class_<TextPacker>(m, "TextPacker")
        .def(py::init<>())
        .def("add", [](TextPacker &self, uint32_t id, uint32_t x, uint32_t y){
            auto result = self.add(id, x, y);
            return py::make_tuple(result.first, result.second);
        })
        .def("remove", &TextPacker::remove);
}