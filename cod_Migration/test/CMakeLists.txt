# Все тестовые файлы
file(GLOB TEST_SOURCES "*.cpp")

# Создаём исполняемый файл тестов
add_executable(tests ${TEST_SOURCES})

# Линковка тестов
target_link_libraries(tests PRIVATE migration_core gtest gtest_main pthread)

# Добавляем цель запуска тестов
add_test(NAME RunTests COMMAND tests)