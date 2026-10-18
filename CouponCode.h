#ifndef COUPON_CODE_H
#define COUPON_CODE_H

#include "IPaymentType.h"

class CouponCode : public IPaymentType {
public:
    int charge(const int& totalCents,
               const std::string& customerId,
               const std::string& authentication) override;
};

#endif
