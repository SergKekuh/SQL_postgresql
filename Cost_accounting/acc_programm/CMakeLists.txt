# Минимальная версия CMake
cmake_minimum_required(VERSION 3.14...3.31)

# Название проекта
project(finance_classifier LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PostgreSQL
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQ REQUIRED libpq)

if(NOT LIBPQ_FOUND)
    message(FATAL_ERROR "libpq not found. Please install PostgreSQL development libraries (libpq-dev).")
endif()

# libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)
set(LIBXL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libxl/include_cpp)  # <-- C++ заголовки

if(NOT EXISTS ${LIBXL_PATH}/libxl.so AND NOT EXISTS ${LIBXL_PATH}/libxl.dll AND NOT EXISTS ${LIBXL_PATH}/libxl.lib)
    message(FATAL_ERROR "⚠️ libxl library not found at ${LIBXL_PATH}")
endif()

# Подключаем заголовочные файлы
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${LIBPQ_INCLUDE_DIRS}
    ${LIBXL_INCLUDE_DIR}
)

# --- ЯДРО ПРОГРАММЫ (библиотека) ---
set(CORE_SOURCES
    src/CategoryClassifier.cpp
    src/PaymentClassifier.cpp
    src/Database.cpp
    # Добавь сюда остальные файлы, если появятся
)

add_library(finance_core STATIC ${CORE_SOURCES})

target_include_directories(finance_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(finance_core PRIVATE
    ${LIBPQ_LIBRARIES}
    ${LIBXL_PATH}/libxl.so
)

# --- ИСПОЛНЯЕМЫЙ ФАЙЛ MAIN ---
add_executable(finance_classifier src/main.cpp)

target_link_libraries(finance_classifier PRIVATE
    finance_core
    ${LIBPQ_LIBRARIES}
    ${LIBXL_PATH}/libxl.so
)

# Настройка флагов компиляции
target_compile_options(finance_core PRIVATE
    $<$<CONFIG:Debug>:-g -Wall -Wextra -O0>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

target_compile_options(finance_classifier PRIVATE
    $<$<CONFIG:Debug>:-g -Wall -Wextra -O0>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

# Тип сборки по умолчанию
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Логирование
message(STATUS "System: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "🟢 libpq include path: ${LIBPQ_INCLUDE_DIRS}")
message(STATUS "🟢 libpq libraries: ${LIBPQ_LIBRARIES}")
message(STATUS "🟢 libxl include path: ${LIBXL_INCLUDE_DIR}")
message(STATUS "🟢 libxl library path: ${LIBXL_PATH}")
message(STATUS "🟢 Build type: ${CMAKE_BUILD_TYPE}")