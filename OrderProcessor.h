#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include "InventoryManager.h"
#include <string>

class OrderProcessor {
public:
    explicit OrderProcessor(InventoryManager& inventory);

    bool placeOrder(int productId, int quantity,
                    const std::string& customerName,
                    const std::string& customerEmail,
                    const std::string& cardLastFour);

private:
    InventoryManager& inventory;
    int nextOrderNumber;
};

#endif
