#include "PaymentTypes.h"
#include "OrderProcessor.h"

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

void restoresPromoCodeRedemptionWhenCheckoutIsRolledBack() {
    PromoCodeLedger promos;
    promos.addCode("THANKYOU25", 2500);
    PromoCodePayment payment("THANKYOU25", promos);
    const PaymentRequest request = requestFor("ada@example.com", 1500);

    assert(payment.process(request).success);
    payment.rollback(request);

    assert(payment.process(request).success);
}

void rejectsUnknownAndInsufficientPromoCodesWithoutRedeemingThem() {
    PromoCodeLedger promos;
    promos.addCode("THANKYOU25", 2500);
    PromoCodePayment unknownPayment("NOT-A-CODE", promos);
    PromoCodePayment payment("THANKYOU25", promos);

    const PaymentResult unknown = unknownPayment.process(requestFor("ada@example.com", 100));
    const PaymentResult insufficient = payment.process(requestFor("ada@example.com", 2600));
    const PaymentResult valid = payment.process(requestFor("ada@example.com", 2500));

    assert(!unknown.success);
    assert(!insufficient.success);
    assert(valid.success);
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

void restoresStoreCreditWhenCheckoutIsRolledBack() {
    StoreCreditLedger credit;
    credit.addCredit("ada@example.com", 3000);
    StoreCreditPayment payment(credit);
    const PaymentRequest request = requestFor("ada@example.com", 1200);

    assert(payment.process(request).success);
    payment.rollback(request);

    assert(credit.balanceFor("ada@example.com") == 3000);
}

class DeclinedPayment : public IPaymentType {
public:
    PaymentResult process(const PaymentRequest&) override {
        return {false, "", "Payment declined for test."};
    }
};

void leavesInventoryUnchangedWhenPaymentFails() {
    InventoryManager inventory;
    OrderProcessor orders(inventory);
    DeclinedPayment payment;
    const int startingQuantity = inventory.products[0].quantity;

    const bool placed = orders.placeOrder(101, 1, "Ada Lovelace", "ada@example.com", payment);

    assert(!placed);
    assert(inventory.products[0].quantity == startingQuantity);
}

}  // namespace

int main() {
    acceptsCreditCardPayment();
    restrictsPromoCodeToOneRedemptionPerCustomer();
    restoresPromoCodeRedemptionWhenCheckoutIsRolledBack();
    rejectsUnknownAndInsufficientPromoCodesWithoutRedeemingThem();
    deductsStoreCreditOnlyWhenTheBalanceCoversTheOrder();
    restoresStoreCreditWhenCheckoutIsRolledBack();
    leavesInventoryUnchangedWhenPaymentFails();

    std::cout << "All payment tests passed.\n";
    return 0;
}
