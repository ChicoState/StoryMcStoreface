#ifndef CREDIT_CARD_H
#define CREDIT_CARD_H

#include "IPaymentType.h"

class CreditCard : public IPaymentType {
public:
    int charge(const int& totalCents,
               const std::string& customerId,
               const std::string& authentication) override;
};

#endif
