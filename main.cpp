#include "InventoryManager.h"
#include "OrderProcessor.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

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

            std::string name, email;
            std::cout << "Customer name: ";
            std::getline(std::cin, name);
            std::cout << "Email (display only; no email is sent): ";
            std::getline(std::cin, email);

            if (name.empty() || email.empty()) {
                std::cout << "Name and email are required.\n";
                continue;
            }

            std::vector<PaymentSelection> payments;
            while (true) {
                int paymentChoice;
                std::cout << "Payment method (1=credit card, 2=coupon code, "
                             "3=store credit, 0=done): ";
                if (!(std::cin >> paymentChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Payment method must be a number.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (paymentChoice == 0) break;
                if (paymentChoice < 1 || paymentChoice > 3) {
                    std::cout << "Unknown payment method.\n";
                    continue;
                }

                PaymentSelection payment;
                payment.method = static_cast<PaymentMethod>(paymentChoice - 1);
                if (paymentChoice == 1) {
                    std::cout << "12 digit card number: ";
                } else if (paymentChoice == 2) {
                    std::cout << "Alphanumeric coupon code: ";
                } else {
                    std::cout << "Available store credit in cents: ";
                    if (!(std::cin >> payment.availableCents)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Store credit must be a number.\n";
                        continue;
                    }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                std::getline(std::cin, payment.authentication);
                payments.push_back(payment);
            }

            orders.placeOrder(productId, quantity, name, email, payments);
        } else {
            std::cout << "Unknown menu choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
