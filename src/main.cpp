#include "db/mail_repository.h"
#include "mail/dry_run_mail_sender.h"
#include "mail/mail_worker.h"

#include <memory>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // TODO(задание 4): выбери путь к базе. Для первого этапа достаточно
    // относительного пути "auto_mailer.sqlite".
    const std::string databasePath = "auto_mailer.sqlite";

    // TODO: здесь напиши код самостоятельно.
    //
    // План:
    // 1. Создай repository через std::make_unique.
    // 2. Подготовь std::unique_ptr<IMailSender> с DryRunMailSender.
    // 3. Создай MailWorker, передав ему databasePath и sender.
    // 4. Запусти std::thread. Удобно переместить worker в lambda:
    //
    //    std::thread workerThread([worker = std::move(worker)]() mutable {
    //        worker->process();
    //    });
    //
    // 5. Для первой консольной версии вызови workerThread.join().
    // 6. Объясни в комментарии, почему detach() здесь использовать опасно.
    // 7. Добавь обработку исключений так, чтобы std::thread не остался
    //    joinable() при выходе из main.

    (void)databasePath;
    return 0;
}
