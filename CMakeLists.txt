cmake_minimum_required(VERSION 3.15)
project(DynamicIsland VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Windows 特定设置
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

# 查找必要的包
find_package(GDIPlus REQUIRED)

# 源文件
set(SOURCES
    src/main.cpp
    src/WindowManager.cpp
    src/Renderer.cpp
    src/TimeService.cpp
    src/ThemeMonitor.cpp
    src/InteractionHandler.cpp
    src/AntiCheatEvasion.cpp
    src/StartupManager.cpp
)

# 头文件
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

# 资源文件
set(RESOURCES
    resources/app.rc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GDIPlus_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    user32
    gdi32
    gdiplus
    shell32
    ole32
    comctl32
    ${GDIPlus_LIBRARIES}
)

# 编译器选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP) # 多处理器编译
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# 优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /O2)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O2)
    endif()
endif()
