#include "PaymentTypes.h"

#include <utility>

namespace {

std::string dollars(int cents) {
    const int remainingCents = cents % 100;
    return "$" + std::to_string(cents / 100) + "." +
           (remainingCents < 10 ? "0" : "") + std::to_string(remainingCents);
}

PaymentResult declined(const std::string& errorMessage) {
    return {false, "", errorMessage};
}

}  // namespace

CreditCardPayment::CreditCardPayment(std::string cardLastFour)
    : cardLastFour(std::move(cardLastFour)) {}

PaymentResult CreditCardPayment::process(const PaymentRequest&) {
    if (cardLastFour.empty()) {
        return declined("Card last four digits are required.");
    }
    return {true, "Credit card (fake last four digits " + cardLastFour + ")", ""};
}

void PromoCodeLedger::addCode(const std::string& code, int valueCents) {
    valuesByCode[code] = valueCents;
}

bool PromoCodeLedger::redeem(const std::string& code, const std::string& customerEmail,
                              int totalCents, std::string& errorMessage) {
    const auto codeEntry = valuesByCode.find(code);
    if (codeEntry == valuesByCode.end()) {
        errorMessage = "Promo code is not recognized.";
        return false;
    }
    if (codeEntry->second < totalCents) {
        errorMessage = "Promo code does not cover the order total.";
        return false;
    }

    const std::string redemptionKey = customerEmail + "\n" + code;
    if (redemptions.count(redemptionKey) != 0) {
        errorMessage = "This promo code has already been redeemed by this customer.";
        return false;
    }

    redemptions.insert(redemptionKey);
    return true;
}

PromoCodePayment::PromoCodePayment(std::string code, PromoCodeLedger& promos)
    : code(std::move(code)), promos(promos) {}

PaymentResult PromoCodePayment::process(const PaymentRequest& request) {
    std::string errorMessage;
    if (!promos.redeem(code, request.customerEmail, request.totalCents, errorMessage)) {
        return declined(errorMessage);
    }
    return {true, "Promo code " + code, ""};
}

void StoreCreditLedger::addCredit(const std::string& customerEmail, int amountCents) {
    balancesByEmail[customerEmail] += amountCents;
}

int StoreCreditLedger::balanceFor(const std::string& customerEmail) const {
    const auto balance = balancesByEmail.find(customerEmail);
    return balance == balancesByEmail.end() ? 0 : balance->second;
}

bool StoreCreditLedger::spend(const std::string& customerEmail, int amountCents) {
    if (balanceFor(customerEmail) < amountCents) {
        return false;
    }
    balancesByEmail[customerEmail] -= amountCents;
    return true;
}

StoreCreditPayment::StoreCreditPayment(StoreCreditLedger& credit) : credit(credit) {}

PaymentResult StoreCreditPayment::process(const PaymentRequest& request) {
    if (!credit.spend(request.customerEmail, request.totalCents)) {
        return declined("Store credit balance does not cover the order total.");
    }
    return {true, "Store credit (" + dollars(request.totalCents) + " redeemed)", ""};
}
