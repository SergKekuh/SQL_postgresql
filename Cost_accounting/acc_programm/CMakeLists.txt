cmake_minimum_required(VERSION 3.14)
project(finance_classifier)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQ REQUIRED libpq)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${LIBPQ_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(finance_classifier ${SOURCES})

target_link_libraries(finance_classifier ${LIBPQ_LIBRARIES})