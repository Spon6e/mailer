#include "mail/dry_run_mail_sender.h"

#include <iostream>

namespace mailer::mail {

SendResult DryRunMailSender::send(const domain::EmailMessage& message)
{
    // TODO(задание 3.1): реализуй безопасный dry-run.
    //
    // Выведи письмо через std::cout или std::cerr. Сеть здесь использовать
    // нельзя. Проверь recipient.email и верни Failed с описанием ошибки,
    // если адрес пустой. Иначе верни Success.
    (void)message;
    return SendResult{
        SendStatus::Failed,
        "DryRunMailSender еще не реализован"};
}

} // namespace mailer::mail
