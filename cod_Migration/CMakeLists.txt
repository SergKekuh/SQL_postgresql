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

# Подключаем заголовочные файлы
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/libxl/include_cpp # ✅ Правильный путь к заголовкам libxl (C++)
)

# Указываем путь к libxl
set(LIBXL_PATH ${PROJECT_SOURCE_DIR}/libxl/lib64)  # ✅ Правильный путь к lib64

# Добавляем исходники из src/
file(GLOB SOURCES "src/*.cpp")
add_executable(main main.cpp ${SOURCES})

# Линковка библиотек
target_link_libraries(main
    PRIVATE
    ${PostgreSQL_LIBRARIES}
    ${LIBXL_PATH}/libxl.so  # ✅ Здесь будет найден libxl.so
)

# RPATH для libxl
set_target_properties(main PROPERTIES
    INSTALL_RPATH "${LIBXL_PATH}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Цель: копировать main в корень проекта
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:main> ${PROJECT_SOURCE_DIR}/main
    COMMENT "Копируем исполняемый файл main в корень проекта"
)

# --- ТЕСТЫ ---
# Включаем поддержку тестов
enable_testing()

# Указываем, где лежат заголовки GTest
include_directories(/usr/src/googletest/googletest/include)

# Создаём исполняемый файл тестов
add_executable(tests test/main.cpp test/database_test.cpp)

# Линковка тестов
target_link_libraries(tests
    PRIVATE
    main
    gtest
    gtest_main
    pthread
)

# Добавляем цель запуска тестов
add_test(NAME RunTests COMMAND tests)