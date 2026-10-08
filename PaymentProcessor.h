#ifndef PAYMENT_PROCESSOR_H
#define PAYMENT_PROCESSOR_H

#include <string>

class PaymentProcessor {
public:
    // No actual charge occurs. Saves a local IOU file instead.
    bool chargeCreditCard(const std::string& customerName,
                          const std::string& cardLastFour,
                          int totalCents,
                          const std::string& orderId);
};

#endif
