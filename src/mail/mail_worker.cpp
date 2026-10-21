#include "mail/mail_worker.h"

#include "db/mail_repository.h"
#include "domain/template_renderer.h"

#include <utility>

namespace mailer::mail {

MailWorker::MailWorker(std::string databasePath, std::unique_ptr<IMailSender> sender)
    : databasePath_(std::move(databasePath))
    , sender_(std::move(sender))
{
    // TODO(задание 4.1): добавь проверку предусловий.
    // sender_ не должен быть nullptr. Реши, использовать ли assert,
    // исключение std::invalid_argument или другой вариант.
}

void MailWorker::process()
{
    // TODO(задание 4.2): реализуй полный цикл обработки очереди.
    //
    // ВАЖНО: этот метод вызывается из std::thread. Репозиторий и SQLite-
    // соединение должны быть созданы внутри этого потока.
    //
    // План:
    // 1. Создай MailRepository с databasePath_ и вызови open().
    // 2. Вызови ensureSchema() и получи pendingTasks().
    // 3. Создай TemplateRenderer<TemplateContext>. Resolver должен
    //    подставлять хотя бы name и email.
    // 4. Для каждой задачи создай копию EmailMessage и отрендери subject/body.
    // 5. Вызови sender_->send().
    // 6. Только после Success вызови repository.markSent().
    // 7. Обработай ошибки и реши, продолжать ли очередь после одной ошибки.
    // 8. Выведи результат обработки каждой задачи.
    //
    // Отдельный вопрос для размышления: что будет, если два std::thread
    // одновременно получат одну pending-задачу? Позже добавим атомарное
    // резервирование записи через транзакцию SQLite.
}

} // namespace mailer::mail
