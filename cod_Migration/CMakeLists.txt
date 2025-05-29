# Минимальная версия CMake (поддерживает диапазон)
cmake_minimum_required(VERSION 3.14...3.31)

# Название проекта
project(PostgreSQLMigrationProject)

# Язык C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Отключаем нестандартные расширения

# Поиск PostgreSQL
find_package(PostgreSQL REQUIRED)

# Добавляем пути к заголовкам
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libxl/include_cpp
)

# Указываем путь к libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)

# Добавляем исходники из src/
file(GLOB SOURCES "src/*.cpp")

# Создаём исполняемый файл
add_executable(main main.cpp ${SOURCES})

# Линковка библиотек
target_link_libraries(main
    PRIVATE
    ${PostgreSQL_LIBRARIES}
    ${LIBXL_PATH}/libxl.so
)

# (Опционально) Можно указать RPATH для libxl.so
set_target_properties(main PROPERTIES
    INSTALL_RPATH "${LIBXL_PATH}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Цель: копировать main в корень проекта
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main> ${PROJECT_SOURCE_DIR}/main
    COMMENT "Копируем main в корень проекта"
)