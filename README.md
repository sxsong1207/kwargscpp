# kwargspp

A simple, header-only C++ library facilitating seamless communication between C++ and Python. This library leverages `std::unordered_map` to emulate Python's `kwargs` with `dict`, supporting various basic data types. It integrates effortlessly with Python using pybind11 and nanobind, making it an ideal choice for developers looking to bridge C++ performance with Python's ease of use.

## Features

- Header-Only: Easy to include in your projects without the need for separate compilation.
- Flexible Data Handling: Utilizes `std::unordered_map` to mimic Python's kwargs, supporting multiple basic data types.
- Seamless Integration: Compatible with both [pybind11](https://pybind11.readthedocs.io/en/stable/) and [nanobind](https://nanobind.readthedocs.io/en/latest/) for Python bindings.
- Lightweight: Minimal dependencies and overhead, ensuring high performance.

## License

This project is licensed under the MIT License.