#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
struct event_struct {
    uint8_t destination;
    uint8_t source;
    uint8_t type;
    uint8_t priority;
    py::object data;
};

PYBIND11_MODULE(event_struct, m) {
    py::class_<event_struct>(m, "event_struct")
        .def(py::init<>())
        .def_readwrite("destination", &event_struct::destination)
        .def_readwrite("priority", &event_struct::priority)
        .def_readwrite("type", &event_struct::type)
        .def_readwrite("data", &event_struct::data)
        .def_readwrite("source", &event_struct::source);
}