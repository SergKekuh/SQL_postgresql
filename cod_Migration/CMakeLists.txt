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

# Подключаем заголовочные файлы
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libxl/include_cpp
    ${PROJECT_SOURCE_DIR}/version
)

# Указываем путь к libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)

# --- ВЕРСИЯ ПРОГРАММЫ ---
add_custom_target(generate_version
    COMMAND ${PROJECT_SOURCE_DIR}/version/generate_version.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Генерируем version.hpp из Git..."
)

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

# Копируем main в корень
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main> ${PROJECT_SOURCE_DIR}/main
    COMMENT "Копируем main в корень проекта"
)

# --- ТЕСТЫ ---
enable_testing()

# Подключаем заголовки GTest
include_directories(/usr/src/googletest/googletest/include)

# Добавляем поддержку тестов
add_subdirectory(test)