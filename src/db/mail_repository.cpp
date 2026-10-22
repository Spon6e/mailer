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
        if (!execute("PRAGMA foreign_keys = ON")) {
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

    if(!execute(createTableRecipients)){
        return false;
    }
    if(!execute(createTableTemplates)){
        return false;
    }
    if(!execute(createTableMail_queue)){
        return false;
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
    return true;
}

bool MailRepository::seedDemoData()
{
    char* errMsg = nullptr;

    const char* demoDataRecipients =
        "INSERT INTO recipients(name, email)"
        "VALUES (?, ?);";

    const char* demoDataTemplates =
        "INSERT INTO templates(nameTemplates, subject, body)"
        "VALUES (?, ?, ?);";

    const char* demoDataMail_queue =
        "INSERT INTO mail_queue(recipient_id, template_id)"
        "VALUES (?, ?);";

    if(sqlite3_exec(databaseHandle_, "BEGIN", nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "error start demoData programm: %s\n", errMsg);
        sqlite3_free(errMsg); //хрень именно от sqlite3, для очистки памяти
        return false;
    }

    const std::string rec = "recipients";
    std::initializer_list<std::pair<int,const char*>> demoDataR = {{0,"Danil"}, {0,"asusmemp10@gmail.com"}};

    const std::string temp = "templates";
    std::initializer_list<std::pair<int,const char*>> demoDataT = {{0, "12"}, {0, "Привет"}, {0, "Спам рассылка просто"}};


    switch(countRecord(rec)){
    case -1:
        rollback();
        return false;
    case 0:
        if(!dataTransmission(demoDataRecipients, demoDataR))
            return false;
        break;
    default:
        break;
    }

    const int countRec = countRecord(rec);

    switch(countRecord(temp)){
    case -1:
        rollback();
        return false;
    case 0:
        if(!dataTransmission(demoDataTemplates, demoDataT))
            return false;
        break;
    default:
        break;
    }

    const int countTemp = countRecord(temp);

    const std::string strCountRec = std::to_string(countRec);
    const std::string strCountTemp = std::to_string(countTemp);
    const std::string mail = "mail_queue";
    std::initializer_list<std::pair<int,const char*>> demoDataM = {{1, strCountRec.c_str()}, {1, strCountTemp.c_str()}};

    switch(countRecord(mail)){
    case -1:
        rollback();
        return false;
    case 0:
        if(!dataTransmission(demoDataMail_queue, demoDataM))
            return false;
        break;
    default:
        break;
    }

    if (sqlite3_exec(databaseHandle_, "COMMIT", nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "error commit demoData programm: %s\n", errMsg);
        sqlite3_free(errMsg);
        rollback();
        return false;
    }
    // TODO(задание 2.5): добавь демонстрационные данные только в пустую БД.
    //
    // Используй sqlite3_prepare_v2(), sqlite3_bind_text(),
    // sqlite3_step() и sqlite3_finalize(). Не склеивай SQL со строками
    // пользователя.
    //
    // Три INSERT должны быть одной транзакцией: BEGIN, INSERT x3, COMMIT.
    // При любой ошибке нужен ROLLBACK.
    return true;
}

std::vector<domain::MailTask> MailRepository::pendingTasks() const
{
    std::vector<domain::MailTask> pendingTasks;

    sqlite3_stmt* stmt = nullptr;

    domain::MailTask data;
    const char* searchWait = "SELECT u.id, u.name, u.email, l.subject, l.body, q.id"
        " FROM mail_queue AS q"
        " JOIN recipients AS u ON q.recipient_id = u.id"
        " JOIN templates AS l ON q.template_id = l.id"
        " WHERE q.status = 'pending' AND u.enabled = 1;";
    if(sqlite3_prepare_v2(databaseHandle_, searchWait, -1, &stmt, nullptr) != SQLITE_OK){
        fprintf(stderr, "We were unable to perform the search: %s\n", sqlite3_errmsg(databaseHandle_));
        sqlite3_finalize(stmt);
        return pendingTasks;
    }

    //ТУТ ИСПРАВИТЬ ХАРДКОД И БРАТЬ ДАННЫЕ ИЗ НАСТРОЕК
    const char* currentEmail = "bikini.th@gmail.com";

    int stepResult;
    while ((stepResult = sqlite3_step(stmt)) == SQLITE_ROW) {
        const std::int64_t id_user = sqlite3_column_int64(stmt, 0);
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* subject = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* body = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        data.id = sqlite3_column_int64(stmt, 5);
        data.message.body = body;
        data.message.subject = subject;
        data.message.from = currentEmail;
        data.message.recipient.email = email;
        data.message.recipient.name = name;
        data.message.recipient.id = id_user;
        pendingTasks.push_back(data);
    }
    if (stepResult != SQLITE_DONE){
        fprintf(stderr, "BAD ENDING STEPS: %s\n", sqlite3_errmsg(databaseHandle_));
    }
    sqlite3_finalize(stmt);
    // TODO(задание 2.6): выполни JOIN-запрос очереди.
    //
    // Выбери pending-задачи только для enabled-получателей. Значения доставай
    // через sqlite3_column_* и копируй в std::string/std::vector.
    // Обязательно вызови sqlite3_finalize() на всех путях выхода.
    return pendingTasks;
}

bool MailRepository::markSent(std::int64_t taskId)
{
    const char* updatingStatus = "UPDATE mail_queue"
        " SET status = ?, sent_at = CURRENT_TIMESTAMP"
        " WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;

    char* errMsg = nullptr;
    if(sqlite3_exec(databaseHandle_, "BEGIN", nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "error start update programm: %s\n", errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    if(sqlite3_prepare_v2(databaseHandle_, updatingStatus, -1, &stmt, nullptr) != SQLITE_OK){
        fprintf(stderr, "NO UPDATE data in table: %s\n", sqlite3_errmsg(databaseHandle_));
        rollback();
        sqlite3_finalize(stmt);
        return false;
    }
    const char* sent = "sent";
    if (sqlite3_bind_text(stmt, 1, sent, -1, SQLITE_TRANSIENT) != SQLITE_OK){
        fprintf(stderr, "NO UPDATE status in table: %s\n", sqlite3_errmsg(databaseHandle_));
        rollback();
        sqlite3_finalize(stmt);
        return false;
    }
    if(sqlite3_bind_int64(stmt, 2, taskId) != SQLITE_OK){
        fprintf(stderr, "NO UPDATE status in table, because bad ID: %s\n", sqlite3_errmsg(databaseHandle_));
        rollback();
        sqlite3_finalize(stmt);
        return false;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE){
        fprintf(stderr, "BAD STEP: %s\n", sqlite3_errmsg(databaseHandle_));
        sqlite3_finalize(stmt);
        rollback();
        return false;
    }
    if(sqlite3_changes(databaseHandle_) != 1){
        fprintf(stderr, "NO updates: %s\n", sqlite3_errmsg(databaseHandle_));
        sqlite3_finalize(stmt);
        rollback();
        return false;
    }
    if (sqlite3_exec(databaseHandle_, "COMMIT", nullptr, nullptr, &errMsg) != SQLITE_OK){
        fprintf(stderr, "error commit update programm: %s\n", errMsg);
        sqlite3_free(errMsg);
        rollback();
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    // TODO(задание 2.7): параметризованным UPDATE переведи задачу в sent.
    // Запиши время в UTC и проверь sqlite3_changes() == 1.
    return true;
}

bool MailRepository::execute(std::string_view sql) const
{
    if(databaseHandle_ == nullptr){
        return false;
    }
    const std::string request(sql);
    char* errorMsg = nullptr;

    if(sqlite3_exec(databaseHandle_, request.c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK){
        fprintf(stderr, "SQLite error: %s\n", errorMsg);
        sqlite3_free(errorMsg);
        return false;
    }
    // TODO(задание 2.8): реализуй вспомогательный метод для DDL/PRAGMA.
    // sqlite3_exec() может вернуть текст ошибки через char*. Не забудь
    // освободить его sqlite3_free().
    return true;
}

void MailRepository::rollback()
{
    char* errMsg = nullptr;
    sqlite3_exec(databaseHandle_, "ROLLBACK", nullptr, nullptr, &errMsg);
    if(errMsg)
        sqlite3_free(errMsg);
}

bool MailRepository::dataTransmission(const char *data, std::initializer_list<std::pair<int, const char *> > fieldsData)
{
    sqlite3_stmt* stmt = nullptr; //указатель на подготовленный оператор
    if (sqlite3_prepare_v2(databaseHandle_, data, -1, &stmt, nullptr) != SQLITE_OK){
        fprintf(stderr, "NO insert data in table: %s\n", sqlite3_errmsg(databaseHandle_));
        rollback();
        return false;
    }
    int counter = 0;
    for (auto i = fieldsData.begin(); i < fieldsData.end(); ++i){
        if(i->first == 1){
            if(sqlite3_bind_int(stmt, ++counter, std::stoi(i->second)) != SQLITE_OK){
                fprintf(stderr, "NO insert data in table: %s\n", sqlite3_errmsg(databaseHandle_));
                rollback();
                sqlite3_finalize(stmt);
                return false;
            }
        }
        else{
            if(sqlite3_bind_text(stmt, ++counter, i->second, -1, SQLITE_TRANSIENT) != SQLITE_OK){
                fprintf(stderr, "NO insert data in table: %s\n", sqlite3_errmsg(databaseHandle_));
                rollback();
                sqlite3_finalize(stmt);
                return false;
            }
        }
    }
    if (sqlite3_step(stmt) != SQLITE_DONE){
        fprintf(stderr, "NO step insert data in table: %s\n", sqlite3_errmsg(databaseHandle_));
        sqlite3_finalize(stmt);
        rollback();
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

int count_callback(void *data, int argc, char **argv, char **)
{
    int *result = static_cast<int*>(data);
    if(argc > 0 && argv[0]){
        *result = std::atoi(argv[0]);
    }
    return 0;
}

int MailRepository::countRecord(const std::string &table_name)
{
    int count = 0;
    char *errorMsg = nullptr;

    const char* requestCountData = sqlite3_mprintf("SELECT COUNT(*) FROM %q", table_name.c_str());
    if(sqlite3_exec(databaseHandle_, requestCountData, count_callback, &count, &errorMsg) != SQLITE_OK){
        fprintf(stderr, "Error request count: %s\n", errorMsg);
        sqlite3_free(const_cast<char*>(requestCountData));
        sqlite3_free(errorMsg);
        return -1;
    }
    sqlite3_free(const_cast<char*>(requestCountData));
    return count;
}



} // namespace mailer::db
