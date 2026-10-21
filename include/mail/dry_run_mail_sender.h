#pragma once

#include "mail/imail_sender.h"

namespace mailer::mail {

class DryRunMailSender final : public IMailSender
{
public:
    SendResult send(const domain::EmailMessage& message) override;
};

} // namespace mailer::mail
