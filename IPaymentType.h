#ifndef IPAYMENT_TYPE_H
#define IPAYMENT_TYPE_H

#include <string>

class IPaymentType {
public:
    virtual ~IPaymentType() = default;

    virtual int charge(const int& totalCents,
                       const std::string& customerId,
                       const std::string& authentication) = 0;
};

#endif
