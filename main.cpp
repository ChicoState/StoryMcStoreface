#include "InventoryManager.h"
#include "OrderProcessor.h"

#include <iostream>
#include <limits>
#include <string>

int main() {
    InventoryManager inventory;
    OrderProcessor orders(inventory);

    std::cout << "Welcome to StoryMcStoreface!\n";

    while (true) {
        std::cout << "\n1. View inventory\n2. Place order\n3. Exit\nChoice: ";
        int choice;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a menu number.\n";
            continue;
        }

        if (choice == 3) break;

        if (choice == 1) {
            // Tight coupling: the user interface reads inventory's array directly.
            for (int i = 0; i < inventory.productCount; ++i) {
                const InventoryManager::Product& item = inventory.products[i];
                std::cout << item.id << " | " << item.name << " | $"
                          << item.priceCents / 100 << '.';
                if (item.priceCents % 100 < 10) std::cout << '0';
                std::cout << item.priceCents % 100
                          << " | In stock: " << item.quantity << '\n';
            }
        } else if (choice == 2) {
            int productId, quantity;
            std::cout << "Product ID: ";
            if (!(std::cin >> productId)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Product ID must be a number.\n";
                continue;
            }
            std::cout << "Quantity: ";
            if (!(std::cin >> quantity)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Quantity must be a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::string name, email, cardLastFour;
            std::cout << "Customer name: ";
            std::getline(std::cin, name);
            std::cout << "Email (display only; no email is sent): ";
            std::getline(std::cin, email);
            std::cout << "FAKE card last four digits (e.g., 1234): ";
            std::getline(std::cin, cardLastFour);

            if (name.empty() || email.empty() || cardLastFour.empty()) {
                std::cout << "All fields are required.\n";
                continue;
            }

            orders.placeOrder(productId, quantity, name, email, cardLastFour);
        } else {
            std::cout << "Unknown menu choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
