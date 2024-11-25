if(NOT nanobind_FOUND)
  message(STATUS "nanobind not found, skipping tests")
  return()
endif()

if(SKBUILD)
  # Enable LTO only for release builds targeting PyPI (~5% binary size reduction)
  set(DIST_FLAGS LTO)
endif()

set(MODULE_NAME bind_nanobind)
nanobind_add_module(${MODULE_NAME} NB_STATIC STABLE_ABI FREE_THREADED ${DIST_FLAGS} bind_nanobind.cpp)

# target_link_libraries(${MODULE_NAME} PRIVATE nanobind)
target_compile_definitions(${MODULE_NAME} PRIVATE VERSION_INFO=${PROJECT_VERSION})

set_target_properties(${MODULE_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../..;${CMAKE_INSTALL_RPATH}")

nanobind_add_stub(${MODULE_NAME}_stub
  MODULE ${MODULE_NAME}
  OUTPUT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${MODULE_NAME}.pyi
  PYTHON_PATH $<TARGET_FILE_DIR:${MODULE_NAME}>
  MARKER_FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/py.typed
  DEPENDS ${MODULE_NAME}
)

add_custom_target(pytest_${MODULE_NAME}
  COMMAND PYTHONPATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests_nanobind.py
  DEPENDS ${MODULE_NAME}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running pytest for ${MODULE_NAME}"
)

add_test(NAME pytest_${MODULE_NAME} COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests_nanobind.py)
set_property(TEST pytest_${MODULE_NAME} PROPERTY ENVIRONMENT_MODIFICATION "PYTHONPATH=set:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
