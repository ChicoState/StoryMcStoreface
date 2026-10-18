#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include "InventoryManager.h"
#include <string>
#include <vector>

enum class PaymentMethod {
    CreditCard,
    CouponCode,
    StoreCredit
};

struct PaymentSelection {
    PaymentMethod method;
    std::string authentication;
    int availableCents = 0;
};

class OrderProcessor {
public:
    explicit OrderProcessor(InventoryManager& inventory);

    bool placeOrder(int productId, int quantity,
                    const std::string& customerName,
                    const std::string& customerEmail,
                    const std::vector<PaymentSelection>& payments);

private:
    InventoryManager& inventory;
    int nextOrderNumber;
};

#endif
