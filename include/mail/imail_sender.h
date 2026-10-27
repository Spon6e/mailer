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

    [[nodiscard]] bool isSuccess() const noexcept //Если мы не будем запоминать рез функции то вылезет ошибка
    {
        return status == SendStatus::Success;
    }
};

class IMailSender
{
public:
    virtual ~IMailSender() = default;
    virtual SendResult send(const domain::EmailMessage& message) = 0;
};

} // namespace mailer::mail
