#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>

class NotificationService {
public:
    void sendEmail(const std::string& recipient,
                   const std::string& subject,
                   const std::string& message);
};

#endif
