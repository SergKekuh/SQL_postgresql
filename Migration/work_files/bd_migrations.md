

---

# 📘 Документация: Создание базы данных `bd_migrations` и настройка pgAdmin

## 1. 🧱 Создание базы данных `bd_migrations`

### ✅ Шаг 1: Вход в PostgreSQL под пользователем `postgres`
```bash
sudo -u postgres psql
```

### ✅ Шаг 2: Создание базы данных
```sql
CREATE DATABASE bd_migrations OWNER = postgres;
```

### ✅ Шаг 3: Выход из `psql`
```sql
\q
```

---

## 2. 🔌 Подключение к базе данных

### ✅ Подключение через `psql`:
```bash
sudo -u postgres psql -d bd_migrations
```

Теперь вы работаете внутри базы данных `bd_migrations`.

---

## 3. 🗂️ Создание таблиц

Вставь следующие SQL-запросы по порядку:

### ✅ Таблица `client_units`

```sql
CREATE TABLE client_units (
  unit_id SERIAL PRIMARY KEY,
  okpo_code VARCHAR(10) NOT NULL UNIQUE,
  okpo_s1c8 VARCHAR(10),
  ipn VARCHAR(10),
  unit_name VARCHAR(255),
  full_unit_name VARCHAR(255),
  legal_entity_type VARCHAR(10),
  note TEXT,

  CONSTRAINT valid_legal_entity CHECK (legal_entity_type IN ('Юр.', 'Фіз.'))
);
```

---

### ✅ Таблица `years`

```sql
CREATE TABLE years (
  year_id SERIAL PRIMARY KEY,
  year INTEGER NOT NULL UNIQUE,
  note TEXT
);
```

---

### ✅ Таблица `groups`

```sql
CREATE TABLE groups (
  group_id SERIAL PRIMARY KEY,
  group_name VARCHAR(50) NOT NULL UNIQUE,
  note TEXT
);
```

---

### ✅ Таблица `client_activity`

```sql
CREATE TABLE client_activity (
  client_activity_id SERIAL PRIMARY KEY,
  unit_id INTEGER NOT NULL REFERENCES client_units(unit_id),
  year_id INTEGER NOT NULL REFERENCES years(year_id),
  group_id INTEGER NOT NULL REFERENCES groups(group_id),
  expense_invoices DECIMAL(15, 2),
  sales DECIMAL(15, 2),
  note TEXT,

  UNIQUE (unit_id, year_id)
);
```

---

## 4. 📐 Проверка структуры таблиц

### ✅ Посмотреть список всех таблиц:
```sql
\dt
```

### ✅ Посмотреть структуру конкретной таблицы:
```sql
\d client_units
```

---

## 5. 🖥️ Установка и настройка pgAdmin

pgAdmin — это мощный инструмент для управления PostgreSQL.

### ✅ Установка pgAdmin (для Ubuntu/Debian):

```bash
sudo apt install pgadmin4
```

Или скачай с официального сайта:  
👉 [https://www.pgadmin.org/download/](https://www.pgadmin.org/download/)

### ✅ Настройка доступа (если нужно):
Правим файл конфигурации:
```bash
sudo nano /etc/postgresql/xx/main/pg_hba.conf
```

Добавляем:
```
local   all             postgres                                peer
local   all             all                                     peer
host    all             all             127.0.0.1/32            md5
host    all             all             ::1/128                 md5
```

Перезапускаем PostgreSQL:
```bash
sudo systemctl restart postgresql
```

### ✅ Запуск pgAdmin:
Зайди по адресу:
```
http://localhost:5050
```

Создай учётную запись администратора.

---

## 6. 🎨 Графическое представление связей (ER Diagram)

### ✅ Открытие ER Diagram в pgAdmin

1. В левом меню выбери:
   ```
   Servers > PostgreSQL > Databases > bd_migrations > Schemas > public
   ```
2. Нажми правой кнопкой мыши → `Diagrams` → `ER Diagram`

### ✅ Возможности:
- Перетаскивай таблицы
- Масштабируй
- Сохраняй диаграмму
- Экспортируй как PNG или SQL

---

## 7. 📝 Пример заполнения таблиц данными

После создания таблиц можно добавить тестовые данные.

### ✅ Добавление записей в `client_units`

```sql
INSERT INTO client_units (okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
VALUES 
('12345678', '87654321', '1234567890', 'ТОВ Альфа', 'Товариство з обмеженою відповідальністю "Альфа"', 'Юр.', 'Основний клієнт'),
('87654321', '12345678', '0987654321', 'ФОП Іванов', 'Фізична особа-підприємець Іванов І.І.', 'Фіз.', 'Додатковий партнер');
```

---

### ✅ Добавление записей в `years`

```sql
INSERT INTO years (year, note)
VALUES 
(2023, 'Рік активного росту'),
(2024, 'Рік експансії');
```

---

### ✅ Добавление записей в `groups`

```sql
INSERT INTO groups (group_name, note)
VALUES 
('Група А', 'Найбільша група'),
('Група Б', 'Середня група');
```

---

### ✅ Добавление записей в `client_activity`

```sql
INSERT INTO client_activity (unit_id, year_id, group_id, expense_invoices, sales, note)
VALUES 
(1, 1, 1, 50000.00, 120000.00, 'Стандартна активність'),
(2, 2, 2, 30000.00, 90000.00, 'Мала активність');
```

---

## 8. 📋 Полезные команды в `psql`
-----------------------------------------------------------------------
| Команда                    | Описание                               |
|----------------------------|----------------------------------------|
| `\dt`                      | Список таблиц                          |
| `\d имя_таблицы`           | Структура таблицы                      |
| `\d+ имя_таблицы`          | Подробная структура                    |
| `\l`                       | Список баз данных                      |
| `\c имя_базы`              | Переключиться на другую БД             |
| `\q`                       | Выйти из psql                          |
-----------------------------------------------------------------------
