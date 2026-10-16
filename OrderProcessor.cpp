#include "OrderProcessor.h"

#include "NotificationService.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

bool savePaymentReceipt(const PaymentRequest& request, const PaymentResult& payment) {
    std::error_code error;
    std::filesystem::create_directories("payments", error);
    if (error) {
        std::cerr << "Could not create payments folder: " << error.message() << '\n';
        return false;
    }

    const std::string filename = "payments/IOU_" + request.orderId + ".txt";
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not create IOU file.\n";
        return false;
    }

    file << "SIMULATED PAYMENT - NO MONEY TRANSFERRED\n";
    file << "Order: " << request.orderId << '\n';
    file << "Customer: " << request.customerName << '\n';
    file << "Payment method: " << payment.receiptDetails << '\n';
    file << "Amount owed: $" << request.totalCents / 100 << '.';
    if (request.totalCents % 100 < 10) file << '0';
    file << request.totalCents % 100 << '\n';
    return static_cast<bool>(file);
}

}  // namespace

OrderProcessor::OrderProcessor(InventoryManager& inventory)
    : inventory(inventory), nextOrderNumber(1) {}

bool OrderProcessor::placeOrder(int productId, int quantity,
                                const std::string& customerName,
                                const std::string& customerEmail,
                                IPaymentType& payment) {
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

    const PaymentRequest request = {orderId, customerName, customerEmail, totalCents};
    const PaymentResult paymentResult = payment.process(request);
    if (!paymentResult.success) {
        std::cout << "Order canceled: " << paymentResult.errorMessage << '\n';
        return false;
    }
    if (!savePaymentReceipt(request, paymentResult)) {
        payment.rollback(request);
        std::cout << "Order canceled: payment record could not be saved.\n";
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
    message << "Payment: " << paymentResult.receiptDetails << "\n";
    message << "An IOU file was saved locally; no real payment was made.";

    NotificationService notifications;
    notifications.sendEmail(customerEmail, "Order confirmation", message.str());
    return true;
}
