#pragma once

#include "domain/email_types.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

struct sqlite3;

namespace mailer::db {

class MailRepository final
{
public:
    // TODO(задание 2): реализуй владение одним SQLite-соединением.
    // Объект репозитория нельзя копировать: соединение должно принадлежать
    // ровно одному экземпляру и использоваться только в создавшем его потоке.
    explicit MailRepository(std::string databasePath);
    ~MailRepository();

    MailRepository(const MailRepository&) = delete;
    MailRepository& operator=(const MailRepository&) = delete;

    bool open();
    bool ensureSchema();
    bool seedDemoData();

    std::vector<domain::MailTask> pendingTasks() const;
    bool markSent(std::int64_t taskId);

private:
    bool execute(std::string_view sql) const;

    std::string databasePath_;

    // TODO(задание 2.1): добавь сюда SQLite-дескриптор после подключения
    // библиотеки sqlite3. Обычно это sqlite3*, которым владеет репозиторий.
    // Не передавай этот указатель между потоками.
    sqlite3* databaseHandle_ = nullptr;
};

} // namespace mailer::db
