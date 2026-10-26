#pragma once

#include "mail/imail_sender.h"

#include <string>
#include <memory>

namespace mailer::mail {

class MailWorker final
{
public:
    // sender передаётся через unique_ptr: MailWorker становится единственным
    // владельцем объекта отправки.
    MailWorker(std::string databasePath, std::unique_ptr<IMailSender> sender);

    // Этот метод будет вызван из std::thread.
    void process();

private:
    std::string databasePath_;
    std::unique_ptr<IMailSender> sender_;
};

class errorForMe{
    std::string err;
public:
    errorForMe(std::string er): err(er){}
    std::string what() const {
        return err;
    }
};
} // namespace mailer::mail
