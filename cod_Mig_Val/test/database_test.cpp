#include "Database.h"
#include <gtest/gtest.h>

// Тест: можно ли создать объект Database
TEST(DatabaseTest, CanCreateObject) {
    // Просто попробуем создать объект — если всё ОК, то компиляция успешна
    bool created = true;
    try {
        Database db("bd_migrations", "postgres", "root");
    } catch (...) {
        created = false;
    }
    EXPECT_TRUE(created);
}

// Тест: можно ли подключиться к БД
TEST(DatabaseTest, CanConnectToDatabase) {
    Database db("bd_migrations", "postgres", "root");

    EXPECT_TRUE(db.isConnected());
}