# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(raylib_imgui_implot)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set variable for raylib directory
set(RAYLIB_DIR "C:/raylib/raylib")

# Add the executable
add_executable(raylib_imgui_implot
    main.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    implot.cpp
    implot_demo.cpp
    implot_items.cpp
    rlImGui.cpp
)

# Include directories
target_include_directories(raylib_imgui_implot PRIVATE ${RAYLIB_DIR}/src)
target_include_directories(raylib_imgui_implot PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link raylib library
# target_link_libraries(raylib_imgui_implot PRIVATE ${RAYLIB_DIR}/src/libraylib.a)
target_link_libraries(raylib_imgui_implot PRIVATE raylib opengl32 winmm gdi32)

# Set the output directory for the executable
set_target_properties(raylib_imgui_implot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message("Build configuration finished")
