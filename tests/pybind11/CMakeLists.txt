if(NOT pybind11_FOUND)
  message(STATUS "nanobind not found, skipping tests")
  return()
endif()

if(SKBUILD)
  # Enable LTO only for release builds targeting PyPI (~5% binary size reduction)
  set(DIST_FLAGS LTO)
endif()

set(MODULE_NAME bind_pybind11)
nanobind_add_module(${MODULE_NAME} NB_STATIC STABLE_ABI FREE_THREADED ${DIST_FLAGS} bind_pybind11.cpp)

target_link_libraries(${MODULE_NAME} PRIVATE kwargscpp)
target_compile_definitions(${MODULE_NAME} PRIVATE VERSION_INFO=${PROJECT_VERSION})

set_target_properties(${MODULE_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../..;${CMAKE_INSTALL_RPATH}")

add_custom_target(pytest_${MODULE_NAME}
  COMMAND PYTHONPATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests_pybind11.py
  DEPENDS ${MODULE_NAME}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running pytest for ${MODULE_NAME}"
)

add_test(NAME pytest_${MODULE_NAME} COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests_pybind11.py)
set_property(TEST pytest_${MODULE_NAME} PROPERTY ENVIRONMENT_MODIFICATION "PYTHONPATH=set:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")