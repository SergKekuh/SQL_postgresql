cmake_minimum_required(VERSION 3.14)
project(finance_classifier)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем пути к libxl
set(LIBXL_ROOT_DIR "${CMAKE_SOURCE_DIR}/libxl")
set(LIBXL_INCLUDE_DIR "${LIBXL_ROOT_DIR}/include_c")
set(LIBXL_LIBRARY_DIR "${LIBXL_ROOT_DIR}/lib")

# Подключаем заголовки libxl
include_directories(${LIBXL_INCLUDE_DIR})

# Подключаем библиотеку
link_directories(${LIBXL_LIBRARY_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQ REQUIRED libpq)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${LIBPQ_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(finance_classifier ${SOURCES})

target_link_libraries(finance_classifier ${LIBPQ_LIBRARIES} xl)