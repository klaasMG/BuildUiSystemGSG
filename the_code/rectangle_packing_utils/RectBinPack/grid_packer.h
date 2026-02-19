#ifndef EVENT_STRUCT_GRID_PACKER_H
#define EVENT_STRUCT_GRID_PACKER_H

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cmath>

struct packed_rect {
    uint16_t x;       // cell coordinates
    uint16_t y;
    uint16_t size_x;  // in cells
    uint16_t size_y;
    uint16_t pixel_size_x;
    uint16_t pixel_size_y;
};

class grid_data {
    std::vector<bool> grid;  // cell occupancy
    std::unordered_map<uint32_t, packed_rect> allocations;

    uint16_t grid_size_x;    // number of cells in x
    uint16_t grid_size_y;    // number of cells in y
    uint16_t cell_size_x;    // pixels per cell
    uint16_t cell_size_y;

public:
    grid_data(uint16_t grid_cells_x,
              uint16_t grid_cells_y,
              uint16_t cell_pixels_x,
              uint16_t cell_pixels_y)
        : grid_size_x(grid_cells_x),
          grid_size_y(grid_cells_y),
          cell_size_x(cell_pixels_x),
          cell_size_y(cell_pixels_y)
    {
        grid.resize(grid_size_x * grid_size_y, false);
    }

    // add a rectangle in pixels
    bool add(uint32_t id, uint32_t pixel_size_x, uint32_t pixel_size_y,
             uint16_t& out_cell_x, uint16_t& out_cell_y)
    {
        if (allocations.find(id) != allocations.end())
            return false;

        // convert pixel size → number of cells, round up
        uint16_t size_x = (pixel_size_x + cell_size_x - 1) / cell_size_x;
        uint16_t size_y = (pixel_size_y + cell_size_y - 1) / cell_size_y;

        if (!find_space(size_x, size_y, out_cell_x, out_cell_y))
            return false;

        mark(out_cell_x, out_cell_y, size_x, size_y, true);

        allocations[id] = { out_cell_x, out_cell_y, size_x, size_y,
                            static_cast<uint16_t>(pixel_size_x),
                            static_cast<uint16_t>(pixel_size_y) };

        return true;
    }

    bool remove(uint32_t id)
    {
        auto it = allocations.find(id);
        if (it == allocations.end())
            return false;

        packed_rect r = it->second;
        mark(r.x, r.y, r.size_x, r.size_y, false);
        allocations.erase(it);

        return true;
    }

private:
    // find empty space in cells
    bool find_space(uint16_t size_x, uint16_t size_y,
                    uint16_t& out_x, uint16_t& out_y)
    {
        if (size_x > grid_size_x || size_y > grid_size_y)
            return false;

        for (uint16_t y = 0; y <= grid_size_y - size_y; ++y) {
            for (uint16_t x = 0; x <= grid_size_x - size_x; ++x) {
                bool fits = true;
                for (uint16_t dy = 0; dy < size_y && fits; ++dy) {
                    for (uint16_t dx = 0; dx < size_x; ++dx) {
                        uint32_t index = (y + dy) * grid_size_x + (x + dx);
                        if (grid[index]) {
                            fits = false;
                            break;
                        }
                    }
                }
                if (fits) {
                    out_x = x;
                    out_y = y;
                    return true;
                }
            }
        }

        return false;
    }

    // mark cells occupied or free
    void mark(uint16_t x, uint16_t y,
              uint16_t size_x, uint16_t size_y, bool value)
    {
        for (uint16_t dy = 0; dy < size_y; ++dy) {
            for (uint16_t dx = 0; dx < size_x; ++dx) {
                uint32_t index = (y + dy) * grid_size_x + (x + dx);
                grid[index] = value;
            }
        }
    }
};

#endif // EVENT_STRUCT_GRID_PACKER_H
