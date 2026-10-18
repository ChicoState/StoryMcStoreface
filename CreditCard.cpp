#include "CreditCard.h"

#include <cctype>

int CreditCard::charge(const int& totalCents,
                       const std::string& customerId,
                       const std::string& authentication) {
    (void)customerId;

    if (totalCents <= 0 || authentication.size() != 12) {
        return 0;
    }

    for (const char digit : authentication) {
        if (!std::isdigit(static_cast<unsigned char>(digit))) {
            return 0;
        }
    }

    return totalCents;
}
