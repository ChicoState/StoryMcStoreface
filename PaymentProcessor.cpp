#include "PaymentProcessor.h"

#include <filesystem>
#include <fstream>
#include <iostream>

bool PaymentProcessor::chargeCreditCard(const std::string& customerName,
                                        const std::string& cardLastFour,
                                        int totalCents,
                                        const std::string& orderId) {
    // Classroom simulation: do not enter real payment information.
    std::error_code error;
    std::filesystem::create_directories("payments", error);
    if (error) {
        std::cerr << "Could not create payments folder: " << error.message() << '\n';
        return false;
    }

    const std::string filename = "payments/IOU_" + orderId + ".txt";
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not create IOU file.\n";
        return false;
    }

    file << "SIMULATED PAYMENT - NO MONEY TRANSFERRED\n";
    file << "Order: " << orderId << '\n';
    file << "Customer: " << customerName << '\n';
    file << "Payment method: Credit card (fake last four digits "
         << cardLastFour << ")\n";
    file << "Amount owed: $" << totalCents / 100 << '.';
    const int cents = totalCents % 100;
    if (cents < 10) file << '0';
    file << cents << '\n';
    file.close();

    if (!file) {
        std::cerr << "Could not finish writing IOU file.\n";
        return false;
    }

    std::cout << "Payment recorded in " << filename << '\n';
    return true;
}
