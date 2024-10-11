# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(cppTewart)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the project base directory
set(PROJECT_BASE_DIR "${CMAKE_SOURCE_DIR}")

# Set variable for raylib directory
set(RAYLIB_DIR "C:/raylib/raylib")

# Add the executable
add_executable(cppTewart
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
    # Stewart platform related source files
    stewart_platform.cpp
    animation.cpp
    stewart_gui.cpp
    stewart_plots.cpp
)

# Include directories
target_include_directories(cppTewart PRIVATE ${RAYLIB_DIR}/src)
target_include_directories(cppTewart PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link raylib library
target_link_libraries(cppTewart PRIVATE ${RAYLIB_DIR}/src/libraylib.a)
target_link_libraries(cppTewart PRIVATE opengl32 winmm gdi32)

# Set the output directory for the executable
set_target_properties(cppTewart PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Pass the project base directory as a preprocessor definition
target_compile_definitions(cppTewart PRIVATE PROJECT_BASE_DIR="${PROJECT_BASE_DIR}")
message("Build configuration finished")
