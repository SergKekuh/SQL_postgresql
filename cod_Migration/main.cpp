#include "Database.h"
#include "ClientUnit.h"

int main() {
    Database db("bd_migrations", "your_user", "your_password");

    if (!db.isConnected()) {
        return 1;
    }

    // Создание таблицы
    ClientUnit::createTable(db);

    // Добавление клиента
    ClientUnit unit(0, "1234567890", "S1C8CODE", "IPN1234567",
                    "Test Company", "Full Legal Name", "Юр.", "Some notes");
    unit.saveToDB(db);

    return 0;
}