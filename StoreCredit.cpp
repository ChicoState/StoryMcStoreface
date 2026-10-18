#include "StoreCredit.h"

#include <algorithm>

StoreCredit::StoreCredit(int availableCents)
    : availableCents(std::max(0, availableCents)) {}

int StoreCredit::charge(const int& totalCents,
                        const std::string& customerId,
                        const std::string& authentication) {
    (void)customerId;

    if (totalCents <= 0 || authentication.empty() || availableCents == 0) {
        return 0;
    }

    const int chargedCents = std::min(totalCents, availableCents);
    availableCents -= chargedCents;
    return chargedCents;
}
