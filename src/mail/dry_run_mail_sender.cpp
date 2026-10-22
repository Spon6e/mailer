#include "mail/dry_run_mail_sender.h"

#include <iostream>

namespace mailer::mail {

SendResult DryRunMailSender::send(const domain::EmailMessage& message)
{
    if(!message.recipient.email.empty()){
        std::cout << "От кого: " << message.from << std::endl;
        std::cout << "Кому: " << message.recipient.email << std::endl;
        std::cout << "Название темы: " << message.subject << std::endl;
        std::cout << "Тело темы: " << message.body << std::endl;
        return SendResult{SendStatus::Success, {}};
    }
    else
    // TODO(задание 3.1): реализуй безопасный dry-run.
    //
    // Выведи письмо через std::cout или std::cerr. Сеть здесь использовать
    // нельзя. Проверь recipient.email и верни Failed с описанием ошибки,
    // если адрес пустой. Иначе верни Success.
        return SendResult{
            SendStatus::Failed,
            "email адрес не указан"};
}

} // namespace mailer::mail
