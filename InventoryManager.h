#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <map>
#include <optional>
#include <string>
#include <vector>

class InventoryManager {
public:
    class Product {
    public:
        int getId() const;
        const std::string& getName() const;
        int getPriceCents() const;
        int getQuantity() const;

    private:
        Product(int id, std::string name, int priceCents, int quantity);
        int id;
        std::string name;
        int priceCents;
        int quantity;
        friend class InventoryManager;
    };

    InventoryManager();
    bool createProduct(int id, const std::string& name, int priceCents, int quantity);
    std::optional<Product> readProduct(int id) const;
    std::vector<Product> readAllProducts() const;
    bool updateProduct(int id, const std::string& name, int priceCents, int quantity);
    bool deleteProduct(int id);
    bool removeQuantity(int id, int quantity);

private:
    std::map<int, Product> products;
};

#endif
