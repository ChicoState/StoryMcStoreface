#include "NotificationService.h"

#include <iostream>

void NotificationService::sendEmail(const std::string& recipient,
                                    const std::string& subject,
                                    const std::string& message) {
    std::cout << "\n--- Email Message ---\n";
    std::cout << "To: " << recipient << '\n';
    std::cout << "Subject: " << subject << '\n';
    std::cout << message << '\n';
    std::cout << "--- End Email Message ---\n";
}
