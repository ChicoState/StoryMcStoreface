#include "InventoryManager.h"

#include <utility>

InventoryManager::Product::Product(int id, std::string name, int priceCents, int quantity)
    : id(id), name(std::move(name)), priceCents(priceCents), quantity(quantity) {}
int InventoryManager::Product::getId() const { return id; }
const std::string& InventoryManager::Product::getName() const { return name; }
int InventoryManager::Product::getPriceCents() const { return priceCents; }
int InventoryManager::Product::getQuantity() const { return quantity; }

InventoryManager::InventoryManager() {
    createProduct(101, "Notebook", 499, 12);
    createProduct(102, "Pen Set", 799, 8);
    createProduct(103, "Water Bottle", 1599, 5);
    createProduct(104, "Backpack", 3499, 3);
}

bool InventoryManager::createProduct(int id, const std::string& name, int priceCents, int quantity) {
    return products.emplace(id, Product(id, name, priceCents, quantity)).second;
}

std::optional<InventoryManager::Product> InventoryManager::readProduct(int id) const {
    const auto product = products.find(id);
    if (product == products.end()) return std::nullopt;
    return product->second;
}

std::vector<InventoryManager::Product> InventoryManager::readAllProducts() const {
    std::vector<Product> result;
    result.reserve(products.size());
    for (const auto& [id, product] : products) result.push_back(product);
    return result;
}

bool InventoryManager::updateProduct(int id, const std::string& name, int priceCents, int quantity) {
    const auto product = products.find(id);
    if (product == products.end()) return false;
    product->second = Product(id, name, priceCents, quantity);
    return true;
}

bool InventoryManager::deleteProduct(int id) {
    return products.erase(id) != 0;
}

bool InventoryManager::removeQuantity(int id, int quantity) {
    const auto product = products.find(id);
    if (product == products.end() || quantity <= 0 || product->second.quantity < quantity) return false;
    product->second.quantity -= quantity;
    return true;
}
