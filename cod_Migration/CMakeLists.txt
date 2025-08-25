# Минимальная версия CMake
cmake_minimum_required(VERSION 3.14...3.31)

# Название проекта
project(PostgreSQLMigrationProject LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PostgreSQL
find_package(PostgreSQL REQUIRED)

if(NOT PostgreSQL_FOUND)
    message(FATAL_ERROR "PostgreSQL not found. Please install PostgreSQL development libraries.")
endif()

# libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)
set(LIBXL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libxl/include_cpp)

if(NOT EXISTS ${LIBXL_PATH}/libxl.so AND NOT EXISTS ${LIBXL_PATH}/libxl.dll AND NOT EXISTS ${LIBXL_PATH}/libxl.lib)
    message(FATAL_ERROR "⚠️ libxl library not found at ${LIBXL_PATH}")
endif()

# Подключаем заголовочные файлы
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${LIBXL_INCLUDE_DIR}
)

# --- ЯДРО ПРОГРАММЫ (библиотека) ---
set(CORE_SOURCES
    src/ClientActivity.cpp
    src/ClientStatistics.cpp
    src/Database.cpp
    src/DatabaseProcedure.cpp
    src/ExcelExporter.cpp
    src/Logger.cpp
    src/StatisticsService.cpp
    # Добавьте остальные файлы
)

add_library(migration_core STATIC ${CORE_SOURCES})

target_include_directories(migration_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(migration_core PRIVATE
    ${PostgreSQL_LIBRARIES}
    ${LIBXL_PATH}/libxl.so
)

# --- ИСПОЛНЯЕМЫЙ ФАЙЛ MAIN ---
add_executable(main main.cpp)

target_link_libraries(main PRIVATE
    migration_core
    ${PostgreSQL_LIBRARIES}
    ${LIBXL_PATH}/libxl.so
)

# Настройка флагов компиляции
target_compile_options(migration_core PRIVATE
    $<$<CONFIG:Debug>:-g -Wall -Wextra -O0>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

target_compile_options(main PRIVATE
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
message(STATUS "🟢 PostgreSQL include path: ${PostgreSQL_INCLUDE_DIRS}")
message(STATUS "🟢 PostgreSQL libraries: ${PostgreSQL_LIBRARIES}")
message(STATUS "🟢 libxl include path: ${LIBXL_INCLUDE_DIR}")
message(STATUS "🟢 libxl library path: ${LIBXL_PATH}")
message(STATUS "🟢 Build type: ${CMAKE_BUILD_TYPE}")