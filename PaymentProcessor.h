#ifndef PAYMENT_PROCESSOR_H
#define PAYMENT_PROCESSOR_H

#include <string>

class PaymentProcessor {
public:
    bool chargeCreditCard(const std::string& customerName,
                          const std::string& cardLastFour,
                          int totalCents,
                          const std::string& orderId);
};

#endif
