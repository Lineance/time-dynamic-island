cmake_minimum_required(VERSION 3.15)
project(DynamicIsland VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows specific settings
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/WindowManager.cpp
    src/Renderer.cpp
    src/TimeService.cpp
    src/ThemeMonitor.cpp
    src/InteractionHandler.cpp
    src/AntiCheatEvasion.cpp
    src/StartupManager.cpp
    src/utils.cpp
)

# Header files
set(HEADERS
    include/WindowManager.h
    include/Renderer.h
    include/TimeService.h
    include/ThemeMonitor.h
    include/InteractionHandler.h
    include/AntiCheatEvasion.h
    include/StartupManager.h
    include/utils.h
)

# Create OBJECT library (no executable, only object files)
add_library(${PROJECT_NAME}_objects OBJECT
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(${PROJECT_NAME}_objects PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Set object file output directory
set_target_properties(${PROJECT_NAME}_objects PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj
)

# Optional: Create a static library from object files
# Uncomment if you want to build a .lib file instead
# add_library(${PROJECT_NAME} STATIC
#     $<TARGET_OBJECTS:${PROJECT_NAME}_objects>
# )
# target_link_libraries(${PROJECT_NAME} PRIVATE
#     user32
#     gdi32
#     gdiplus
#     shell32
#     ole32
#     comctl32
# )

message(STATUS "Building OBJECT files only - No executable will be created")
message(STATUS "Object files will be in: ${CMAKE_BINARY_DIR}/obj")
