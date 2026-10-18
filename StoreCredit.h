#ifndef STORE_CREDIT_H
#define STORE_CREDIT_H

#include "IPaymentType.h"

class StoreCredit : public IPaymentType {
public:
    explicit StoreCredit(int availableCents);

    int charge(const int& totalCents,
               const std::string& customerId,
               const std::string& authentication) override;

private:
    int availableCents;
};

#endif
