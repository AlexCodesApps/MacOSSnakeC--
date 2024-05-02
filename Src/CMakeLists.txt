cmake_minimum_required(VERSION 3.26)
project(Snake)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED COMPONENTS SDL2_image)

# Add the source files to the executable
file(GLOB_RECURSE SRC_FILES *.cpp)

# Add the executable
add_executable(Snake MACOSX_BUNDLE ${SRC_FILES})

# Copy the icon file to the Snake.app bundle
add_custom_command(
    TARGET Snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/../icon.png
        ${CMAKE_BINARY_DIR}/../Snake.app/Contents/Resources/Icon.png
)

# Add the icon file to the Info.plist file
set(MACOSX_BUNDLE_ICON_FILE Icon.png)
set_source_files_properties(${CMAKE_BINARY_DIR}/../Snake.app/Contents/Resources/Icon.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

# Set the path to the Info.plist file
set_target_properties(Snake PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/../Snake.app/Contents/Info.plist)

# Link SDL2 and SDL2_image libraries
target_link_libraries(Snake PRIVATE SDL2::SDL2 PRIVATE SDL2_image::SDL2_image)

