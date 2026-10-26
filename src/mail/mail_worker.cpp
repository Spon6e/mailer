#include "mail/mail_worker.h"

#include "db/mail_repository.h"
#include "domain/template_renderer.h"

#include <utility>
#include <cassert>
#include <iostream>

namespace mailer::mail {

MailWorker::MailWorker(std::string databasePath, std::unique_ptr<IMailSender> sender)
    : databasePath_(std::move(databasePath))
    , sender_(std::move(sender))
{
    assert(sender_ != nullptr);
    // TODO(задание 4.1): добавь проверку предусловий.
    // sender_ не должен быть nullptr. Реши, использовать ли assert,
    // исключение std::invalid_argument или другой вариант.
}

void MailWorker::process()
{
    try{
        db::MailRepository reposit(databasePath_);
        if(!reposit.open()){
            throw errorForMe("Ошибка открытия БД");
        }
        if(!reposit.ensureSchema()){
            throw errorForMe("Ошибка создания таблиц БД");
        }
        std::vector<domain::MailTask> myVec = reposit.pendingTasks();
        if(myVec.empty()){
            std::cout << "Список задач пуст" << std::endl;
            return;
        }

        domain::TemplateContext dataPerson;
        domain::TemplateRenderer<domain::TemplateContext> upgrader([](const domain::TemplateContext& a, std::string_view b){
            if(b == "name"){
                return a.recipientName;
            }
            if(b == "email"){
                return a.recipientEmail;
            }
            return std::string{};
        });

        for(auto i = myVec.begin(); i != myVec.end(); ++i){
            dataPerson.recipientEmail = i->message.recipient.email;
            dataPerson.recipientName = i->message.recipient.name;
            domain::EmailMessage SMS = i->message;
            SMS.body = upgrader.render(i->message.body, dataPerson);
            SMS.subject = upgrader.render(i->message.subject, dataPerson);
            if(!sender_->send(SMS).isSuccess()){
                std::cout << "Ошибка отправки сообщения пользователю: " << dataPerson.recipientName << ", email: " << dataPerson.recipientEmail << std::endl;
                continue;
            }
            else{
                if(!reposit.markSent(i->id)){
                    std::cout << "Ошибка обновления статуса пользователю: " << dataPerson.recipientName << ", email: " << dataPerson.recipientEmail << std::endl;
                    continue;
                }
            }
            std::cout << "Задача успешно обработана. Пользователь: " << dataPerson.recipientName << ", email: " << dataPerson.recipientEmail << std::endl;
        }

        return;
    }
    catch(const errorForMe & error){
        std::cout << error.what() << std::endl;
        return;
    }

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
