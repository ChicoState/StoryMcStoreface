#include "CouponCode.h"

#include <cctype>

int CouponCode::charge(const int& totalCents,
                       const std::string& customerId,
                       const std::string& authentication) {
    (void)customerId;

    if (totalCents <= 0 || authentication.empty()) {
        return 0;
    }

    for (const char character : authentication) {
        if (!std::isalnum(static_cast<unsigned char>(character))) {
            return 0;
        }
    }

    return totalCents;
}
