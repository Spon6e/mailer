#include "db/mail_repository.h"

#include <utility>
#include <sqlite3.h>

namespace mailer::db {

MailRepository::MailRepository(std::string databasePath)
    : databasePath_(std::move(databasePath))
{
    // TODO(задание 2.1): подключи sqlite3.h в этом .cpp и реализуй открытие
    // соединения в методе open(). Сам указатель databaseHandle_ уже объявлен
    // как sqlite3* без зависимости от Qt.
}

MailRepository::~MailRepository()
{
    if(databaseHandle_ != nullptr){
        sqlite3_close(databaseHandle_);
        databaseHandle_ = nullptr;
    }
    // TODO(задание 2.2): если databaseHandle_ не nullptr, вызови
    // sqlite3_close(databaseHandle_). После этого обнули указатель.
    //
    // В будущем здесь нужно будет проверить, что все подготовленные
    // sqlite3_stmt уже освобождены через sqlite3_finalize().
}

bool MailRepository::open()
{
    if(sqlite3_open(databasePath_.c_str(), &databaseHandle_) == SQLITE_OK){
        sqlite3_busy_timeout(databaseHandle_, 5000);
        char* errorMsg = nullptr;
        if (sqlite3_exec(databaseHandle_, "PRAGMA foreign_keys = ON", nullptr, nullptr, &errorMsg) != SQLITE_OK){
            fprintf(stderr, "Pragma error: %s\n", errorMsg);
            sqlite3_free(errorMsg);
            return false;
        }
        return true;
    }
    else{
        fprintf(stderr, "NO OPEN database: %s\n", sqlite3_errmsg(databaseHandle_));
        return false;
    }

    // TODO(задание 2.3): реализуй открытие SQLite-базы.
    //
    // 1. Добавь в CMake зависимость от SQLite3.
    // 2. Подключи <sqlite3.h> только в .cpp-файле.
    // 3. Вызови sqlite3_open(databasePath_.c_str(), &databaseHandle_).
    // 4. Проверь код возврата и выведи sqlite3_errmsg() при ошибке.
    // 5. Настрой sqlite3_busy_timeout().
    // 6. Реши, нужен ли PRAGMA foreign_keys = ON, и включи его.
}

bool MailRepository::ensureSchema()
{
    const char* createTableRecipients =
        "CREATE TABLE IF NOT EXISTS recipients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "enabled INTEGER NOT NULL DEFAULT 1);";

    const char* createTableTemplates =
        "CREATE TABLE IF NOT EXISTS templates("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nameTemplates TEXT NOT NULL,"
        "subject TEXT NOT NULL,"
        "body TEXT NOT NULL);";

    const char* createTableMail_queue =
        "CREATE TABLE IF NOT EXISTS mail_queue ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "recipient_id INTEGER NOT NULL,"
        "template_id INTEGER NOT NULL,"
        "status TEXT NOT NULL DEFAULT 'pending',"
        "created_at TEXT DEFAULT (datetime('now')),"
        "sent_at TEXT,"
        "FOREIGN KEY (recipient_id) REFERENCES recipients(id),"
        "FOREIGN KEY (template_id) REFERENCES templates(id));";

    char* errMsg = nullptr;

    bool cool = true;
    if(sqlite3_exec(databaseHandle_, createTableRecipients, nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "NO CREATE users: %s\n", errMsg);
        sqlite3_free(errMsg);
        cool = false;
    }
    if(sqlite3_exec(databaseHandle_, createTableTemplates, nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "NO CREATE templates: %s\n", errMsg);
        sqlite3_free(errMsg);
        cool = false;
    }
    if(sqlite3_exec(databaseHandle_, createTableMail_queue, nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "NO CREATE mail_queue: %s\n", errMsg);
        sqlite3_free(errMsg);
        cool = false;
    }
    // TODO(задание 2.4): создай таблицы recipients, templates и mail_queue
    // через sqlite3_exec() или подготовленные DDL-запросы.
    //
    // Минимальная схема:
    // recipients(id, name, email, enabled)
    // templates(id, name, subject, body)
    // mail_queue(id, recipient_id, template_id, status, created_at, sent_at)
    //
    // Добавь PRIMARY KEY, UNIQUE для email, FOREIGN KEY и DEFAULT status.
    // Не игнорируй код возврата SQLite.
    return cool;
}

bool MailRepository::seedDemoData()
{
    const char* demoDataRecipients =
        "INSERT INTO recipients(name, email)"
        "VALUE ('Danil', 'asusmemp10@gmail.com');";

    const char* demoDataTemplates =
        "INSERT INTO templates(nameTemplates, subject, body)"
        "VALUE ('Seedlings', 'Hello', '');";

    const char* demoDataMail_queue =
        "INSERT INTO mail_queue(name, email)"
        "VALUE ('Danil', 'asusmemp10@gmail.com');";
    // TODO(задание 2.5): добавь демонстрационные данные только в пустую БД.
    //
    // Используй sqlite3_prepare_v2(), sqlite3_bind_text(),
    // sqlite3_step() и sqlite3_finalize(). Не склеивай SQL со строками
    // пользователя.
    //
    // Три INSERT должны быть одной транзакцией: BEGIN, INSERT x3, COMMIT.
    // При любой ошибке нужен ROLLBACK.
    return false;
}

std::vector<domain::MailTask> MailRepository::pendingTasks() const
{
    // TODO(задание 2.6): выполни JOIN-запрос очереди.
    //
    // Выбери pending-задачи только для enabled-получателей. Значения доставай
    // через sqlite3_column_* и копируй в std::string/std::vector.
    // Обязательно вызови sqlite3_finalize() на всех путях выхода.
    return {};
}

bool MailRepository::markSent(std::int64_t taskId)
{
    // TODO(задание 2.7): параметризованным UPDATE переведи задачу в sent.
    // Запиши время в UTC и проверь sqlite3_changes() == 1.
    (void)taskId;
    return false;
}

bool MailRepository::execute(std::string_view sql) const
{
    // TODO(задание 2.8): реализуй вспомогательный метод для DDL/PRAGMA.
    // sqlite3_exec() может вернуть текст ошибки через char*. Не забудь
    // освободить его sqlite3_free().
    (void)sql;
    return false;
}

} // namespace mailer::db
