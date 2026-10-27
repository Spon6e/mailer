#include "db/mail_repository.h"
#include "mail/dry_run_mail_sender.h"
#include "mail/mail_worker.h"

#include <memory>
#include <string>
#include <thread>
#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    const std::string databasePath = "auto_mailer.sqlite";

    auto sender = std::make_unique<mailer::mail::DryRunMailSender>();
    auto worker = std::make_unique<mailer::mail::MailWorker>(databasePath, std::move(sender));

    std::thread workerThread([worker = std::move(worker)]() mutable{
        try{
            worker->process();
        }
        catch(const mailer::mail::errorForMe& er){
            std::cout << "Ошибка " << er.what() << std::endl;
        }
        catch(const std::exception& error){
            std::cout << error.what();
        }
    });
    if(workerThread.joinable()){
        workerThread.join();
    }

    return 0;
}
