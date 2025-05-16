# Версия CMake
cmake_minimum_required(VERSION 3.14)

# Название проекта
project(PostgreSQLMigrationProject)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ищем библиотеку PostgreSQL (libpq)
find_package(PQ REQUIRED)

# Добавляем заголовочные пути libpq
include_directories(${PQ_INCLUDE_DIRS})

# Добавляем исполняемый файл
add_executable(main main.cpp)

# Линкуем с библиотекой PostgreSQL
target_link_libraries(main ${PQ_LIBRARIES})