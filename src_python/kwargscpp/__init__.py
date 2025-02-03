def get_cmake_dir() -> str:
    "Return the path to the Dr.Jit CMake module directory."
    import os
    return os.path.join(os.path.abspath(os.path.dirname(__file__)), "lib", "cmake", "kwargscpp")