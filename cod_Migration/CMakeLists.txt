# Минимальная версия CMake
cmake_minimum_required(VERSION 3.14...3.31)

# Название проекта
project(PostgreSQLMigrationProject)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PostgreSQL
find_package(PostgreSQL REQUIRED)

# Подключение GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Подключаем заголовочные файлы
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libxl/include_cpp
    ${PROJECT_SOURCE_DIR}/version
    ${GTEST_INCLUDE_DIRS}   #<-- Это заменяет /usr/src/googletest/googletest/include
)

# Указываем путь к libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)

# --- ВЕРСИЯ ПРОГРАММЫ ---
add_custom_target(generate_version
    COMMAND ${PROJECT_SOURCE_DIR}/version/generate_version.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}    
    BYPRODUCTS ${PROJECT_SOURCE_DIR}/version/version.hpp
    COMMENT "Генерируем version.hpp из Git..."
)

# Выполняем generate_version перед main
add_dependencies(main generate_version)

# Подключаем заголовки с версией
include_directories(${PROJECT_SOURCE_DIR}/version)

# --- ЯДРО ПРОГРАММЫ (библиотека) ---
file(GLOB CORE_SOURCES "src/*.cpp")
add_library(migration_core STATIC ${CORE_SOURCES})

target_include_directories(migration_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(migration_core PRIVATE ${PostgreSQL_LIBRARIES} ${LIBXL_PATH}/libxl.so)

# --- ИСПОЛНЯЕМЫЙ ФАЙЛ MAIN ---
add_executable(main main.cpp)
add_dependencies(main generate_version)
target_link_libraries(main PRIVATE migration_core)

# Получаем версию из файла
file(READ ${PROJECT_SOURCE_DIR}/version/version.hpp APP_VERSION_CONTENT)
string(REGEX MATCH "APP_VERSION \"([^\"]+)\"" _ ${APP_VERSION_CONTENT})
set(APP_VERSION ${CMAKE_MATCH_1})

# Переименовываем исполняемый файл с версией
set_target_properties(main PROPERTIES OUTPUT_NAME "main_${APP_VERSION}")

# Копируем main в корень проекта с версией
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main> ${PROJECT_SOURCE_DIR}/main_${APP_VERSION}
    COMMENT "Копируем main с версией в корень проекта"
)

# --- ТЕСТЫ ---

enable_testing()
#include_directories(/usr/src/googletest/googletest/include)
add_subdirectory(test)

message(STATUS "GTest include path: ${GTEST_INCLUDE_DIRS}")
message(STATUS "GTest libraries: ${GTEST_LIBRARIES}")