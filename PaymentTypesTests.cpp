#include "PaymentTypes.h"

#include <cassert>
#include <iostream>

namespace {

PaymentRequest requestFor(const std::string& email, int amountCents) {
    return {"order-1", "Ada Lovelace", email, amountCents};
}

void acceptsCreditCardPayment() {
    CreditCardPayment payment("1234");

    const PaymentResult result = payment.process(requestFor("ada@example.com", 499));

    assert(result.success);
    assert(result.receiptDetails.find("1234") != std::string::npos);
}

void restrictsPromoCodeToOneRedemptionPerCustomer() {
    PromoCodeLedger promos;
    promos.addCode("THANKYOU25", 2500);
    PromoCodePayment payment("THANKYOU25", promos);

    const PaymentResult first = payment.process(requestFor("ada@example.com", 1500));
    const PaymentResult second = payment.process(requestFor("ada@example.com", 1500));

    assert(first.success);
    assert(!second.success);
}

void deductsStoreCreditOnlyWhenTheBalanceCoversTheOrder() {
    StoreCreditLedger credit;
    credit.addCredit("ada@example.com", 3000);
    StoreCreditPayment payment(credit);

    const PaymentResult approved = payment.process(requestFor("ada@example.com", 1200));
    const PaymentResult declined = payment.process(requestFor("ada@example.com", 2000));

    assert(approved.success);
    assert(credit.balanceFor("ada@example.com") == 1800);
    assert(!declined.success);
    assert(credit.balanceFor("ada@example.com") == 1800);
}

}  // namespace

int main() {
    acceptsCreditCardPayment();
    restrictsPromoCodeToOneRedemptionPerCustomer();
    deductsStoreCreditOnlyWhenTheBalanceCoversTheOrder();

    std::cout << "All payment tests passed.\n";
    return 0;
}
