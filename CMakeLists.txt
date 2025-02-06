cmake_minimum_required(VERSION 3.21)
project(IngameListMod VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

# Tìm tất cả các tệp nguồn trong thư mục src
file(GLOB SOURCES
    src/*.cpp
)

# Tạo thư viện chia sẻ
add_library(${PROJECT_NAME} SHARED ${SOURCES} "src/include.h")

# Thêm thư mục include
target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/includes/
)

# Kiểm tra biến môi trường GEODE_SDK
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

# Thêm thư mục Geode SDK
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Thiết lập mod Geode
setup_geode_mod(${PROJECT_NAME})
