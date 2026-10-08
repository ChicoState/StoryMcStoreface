#include "InventoryManager.h"

InventoryManager::InventoryManager() : productCount(4) {
    products[0] = {101, "Notebook", 499, 12};
    products[1] = {102, "Pen Set", 799, 8};
    products[2] = {103, "Water Bottle", 1599, 5};
    products[3] = {104, "Backpack", 3499, 3};
}
