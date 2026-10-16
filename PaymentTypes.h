#ifndef PAYMENT_TYPES_H
#define PAYMENT_TYPES_H

#include <map>
#include <set>
#include <string>

struct PaymentRequest {
    std::string orderId;
    std::string customerName;
    std::string customerEmail;
    int totalCents;
};

struct PaymentResult {
    bool success;
    std::string receiptDetails;
    std::string errorMessage;
};

class IPaymentType {
public:
    virtual ~IPaymentType() = default;
    virtual PaymentResult process(const PaymentRequest& request) = 0;
    virtual void rollback(const PaymentRequest&) {}
};

class CreditCardPayment : public IPaymentType {
public:
    explicit CreditCardPayment(std::string cardLastFour);
    PaymentResult process(const PaymentRequest& request) override;

private:
    std::string cardLastFour;
};

class PromoCodeLedger {
public:
    void addCode(const std::string& code, int valueCents);
    bool redeem(const std::string& code, const std::string& customerEmail,
                int totalCents, std::string& errorMessage);
    void undoRedemption(const std::string& code, const std::string& customerEmail);

private:
    std::map<std::string, int> valuesByCode;
    std::set<std::string> redemptions;
};

class PromoCodePayment : public IPaymentType {
public:
    PromoCodePayment(std::string code, PromoCodeLedger& promos);
    PaymentResult process(const PaymentRequest& request) override;
    void rollback(const PaymentRequest& request) override;

private:
    std::string code;
    PromoCodeLedger& promos;
    bool processed = false;
};

class StoreCreditLedger {
public:
    void addCredit(const std::string& customerEmail, int amountCents);
    int balanceFor(const std::string& customerEmail) const;
    bool spend(const std::string& customerEmail, int amountCents);

private:
    std::map<std::string, int> balancesByEmail;
};

class StoreCreditPayment : public IPaymentType {
public:
    explicit StoreCreditPayment(StoreCreditLedger& credit);
    PaymentResult process(const PaymentRequest& request) override;
    void rollback(const PaymentRequest& request) override;

private:
    StoreCreditLedger& credit;
    bool processed = false;
};

#endif
