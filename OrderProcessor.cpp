#include "OrderProcessor.h"

#include "CouponCode.h"
#include "CreditCard.h"
#include "NotificationService.h"
#include "StoreCredit.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

OrderProcessor::OrderProcessor(InventoryManager& inventory)
    : inventory(inventory), nextOrderNumber(1) {}

bool OrderProcessor::placeOrder(int productId, int quantity,
                                const std::string& customerName,
                                const std::string& customerEmail,
                                const std::vector<PaymentSelection>& payments) {
    if (quantity <= 0) {
        std::cout << "Quantity must be positive.\n";
        return false;
    }

    int productIndex = -1;
    for (int i = 0; i < inventory.productCount; ++i) {
        if (inventory.products[i].id == productId) {
            productIndex = i;
            break;
        }
    }

    if (productIndex == -1) {
        std::cout << "Product not found.\n";
        return false;
    }

    if (inventory.products[productIndex].quantity < quantity) {
        std::cout << "Not enough stock available.\n";
        return false;
    }

    const int totalCents = inventory.products[productIndex].priceCents * quantity;
    const std::string productName = inventory.products[productIndex].name;

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    const std::string orderId = std::to_string(timestamp) + "_" +
                                std::to_string(nextOrderNumber++);

    int remainingCents = totalCents;
    for (const PaymentSelection& selection : payments) {
        if (remainingCents == 0) {
            break;
        }

        std::unique_ptr<IPaymentType> payment;
        switch (selection.method) {
            case PaymentMethod::CreditCard:
                payment = std::make_unique<CreditCard>();
                break;
            case PaymentMethod::CouponCode:
                payment = std::make_unique<CouponCode>();
                break;
            case PaymentMethod::StoreCredit:
                payment = std::make_unique<StoreCredit>(selection.availableCents);
                break;
        }

        const int chargedCents = payment->charge(
            remainingCents, customerEmail, selection.authentication);
        if (chargedCents <= 0 || chargedCents > remainingCents) {
            std::cout << "Order canceled: payment could not be completed.\n";
            return false;
        }
        remainingCents -= chargedCents;
    }

    if (remainingCents > 0) {
        std::cout << "Order canceled: selected payment methods did not cover the total.\n";
        return false;
    }

    // Tight coupling: directly changes InventoryManager's internal array.
    inventory.products[productIndex].quantity -= quantity;

    // Tight coupling: assumes the only notification is an email.
    std::ostringstream message;
    message << "Hello " << customerName << ",\n";
    message << "Order " << orderId << " has been recorded.\n";
    message << "Item: " << productName << "\n";
    message << "Quantity: " << quantity << "\n";
    message << "Total: $" << totalCents / 100 << '.';
    if (totalCents % 100 < 10) message << '0';
    message << totalCents % 100 << "\n";
    message << "Payment was simulated locally; no real payment was made.";

    NotificationService notifications;
    notifications.sendEmail(customerEmail, "Order confirmation", message.str());
    return true;
}
