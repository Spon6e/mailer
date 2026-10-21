#pragma once

#include <cstdint>
#include <string>

namespace mailer::domain {

struct Recipient final
{
    std::int64_t id = 0;
    std::string name;
    std::string email;
};

struct EmailMessage final
{
    std::string from;
    Recipient recipient;
    std::string subject;
    std::string body;
};

struct MailTask final
{
    std::int64_t id = 0;
    EmailMessage message;
};

struct TemplateContext final
{
    std::string recipientName;
    std::string recipientEmail;
};

} // namespace mailer::domain
