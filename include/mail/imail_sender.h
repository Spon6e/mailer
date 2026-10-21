#pragma once

#include "domain/email_types.h"

#include <string>

namespace mailer::mail {

enum class SendStatus
{
    Success,
    Failed
};

struct SendResult final
{
    SendStatus status = SendStatus::Failed;
    std::string errorMessage;

    [[nodiscard]] bool isSuccess() const noexcept
    {
        return status == SendStatus::Success;
    }
};

class IMailSender
{
public:
    // TODO(задание 3): создай реализацию интерфейса для dry-run, а позже —
    // отдельную реализацию настоящего SMTP/API-отправителя.
    // Виртуальный деструктор нужен, потому что объект будет удаляться через
    // указатель на базовый класс.
    virtual ~IMailSender() = default;
    virtual SendResult send(const domain::EmailMessage& message) = 0;
};

} // namespace mailer::mail
