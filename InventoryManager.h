#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <string>

class InventoryManager {
public:
    struct Product {
        int id;
        std::string name;
        int priceCents;
        int quantity;
    };

    static const int CAPACITY = 5;
    Product products[CAPACITY];
    int productCount;

    InventoryManager();
};

#endif
