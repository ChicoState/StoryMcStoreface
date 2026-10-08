#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>

class NotificationService {
public:
    // Classroom simulation: prints email instead of sending one.
    void sendEmail(const std::string& recipient,
                   const std::string& subject,
                   const std::string& message);
};

#endif
