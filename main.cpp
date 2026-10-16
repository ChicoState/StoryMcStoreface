#include "InventoryManager.h"
#include "OrderProcessor.h"

#include <iostream>
#include <limits>
#include <memory>
#include <string>

int main() {
    InventoryManager inventory;
    OrderProcessor orders(inventory);
    PromoCodeLedger promoCodes;
    promoCodes.addCode("THANKYOU25", 2500);
    StoreCreditLedger storeCredit;
    storeCredit.addCredit("storecredit@example.com", 5000);

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
                std::cout << "Customer name and email are required.\n";
                continue;
            }

            std::cout << "Payment method:\n"
                      << "1. Credit card\n"
                      << "2. Promo code\n"
                      << "3. Store credit\n"
                      << "Choice: ";
            int paymentChoice;
            if (!(std::cin >> paymentChoice)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Payment method must be a number.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::unique_ptr<IPaymentType> payment;
            if (paymentChoice == 1) {
                std::string cardLastFour;
                std::cout << "FAKE card last four digits (e.g., 1234): ";
                std::getline(std::cin, cardLastFour);
                payment = std::make_unique<CreditCardPayment>(cardLastFour);
            } else if (paymentChoice == 2) {
                std::string promoCode;
                std::cout << "Promo code: ";
                std::getline(std::cin, promoCode);
                payment = std::make_unique<PromoCodePayment>(promoCode, promoCodes);
            } else if (paymentChoice == 3) {
                payment = std::make_unique<StoreCreditPayment>(storeCredit);
            } else {
                std::cout << "Unknown payment method.\n";
                continue;
            }

            orders.placeOrder(productId, quantity, name, email, *payment);
        } else {
            std::cout << "Unknown menu choice.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}
