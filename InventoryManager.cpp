#include "InventoryManager.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

struct ParsedProduct {
    int id;
    std::string name;
    int priceCents;
    int quantity;
};

class JsonParser {
public:
    explicit JsonParser(std::string text) : text(std::move(text)) {}

    std::vector<ParsedProduct> parseProducts() {
        std::vector<ParsedProduct> result;
        expect('[');
        skipWhitespace();
        if (take(']')) return result;

        while (true) {
            expect('{');
            skipWhitespace();
            int id = 0;
            std::string name;
            int priceCents = 0;
            int quantity = 0;
            bool hasId = false;
            bool hasName = false;
            bool hasPrice = false;
            bool hasQuantity = false;

            while (true) {
                if (take('}')) break;
                const std::string key = parseString();
                expect(':');
                if (key == "id") {
                    id = parseInteger();
                    hasId = true;
                } else if (key == "name") {
                    name = parseString();
                    hasName = true;
                } else if (key == "priceCents") {
                    priceCents = parseInteger();
                    hasPrice = true;
                } else if (key == "quantity") {
                    quantity = parseInteger();
                    hasQuantity = true;
                } else {
                    throw std::runtime_error("Unknown inventory field: " + key);
                }
                skipWhitespace();
                if (take('}')) break;
                expect(',');
            }

            if (!hasId || !hasName || !hasPrice || !hasQuantity) {
                throw std::runtime_error("Invalid inventory product");
            }
            for (const auto& product : result) {
                if (product.id == id) throw std::runtime_error("Duplicate inventory product id");
            }
            result.push_back({id, name, priceCents, quantity});

            skipWhitespace();
            if (take(']')) break;
            expect(',');
        }

        skipWhitespace();
        if (position != text.size()) throw std::runtime_error("Unexpected inventory data");
        return result;
    }

private:
    void skipWhitespace() {
        while (position < text.size() && std::isspace(static_cast<unsigned char>(text[position]))) ++position;
    }

    bool take(char expected) {
        skipWhitespace();
        if (position < text.size() && text[position] == expected) {
            ++position;
            return true;
        }
        return false;
    }

    void expect(char expected) {
        if (!take(expected)) throw std::runtime_error("Invalid inventory JSON");
    }

    std::string parseString() {
        expect('"');
        std::string result;
        while (position < text.size()) {
            const char character = text[position++];
            if (character == '"') return result;
            if (character == '\\') {
                if (position >= text.size()) break;
                const char escaped = text[position++];
                if (escaped == '"' || escaped == '\\' || escaped == '/') result += escaped;
                else if (escaped == 'n') result += '\n';
                else if (escaped == 'r') result += '\r';
                else if (escaped == 't') result += '\t';
                else throw std::runtime_error("Unsupported inventory string escape");
            } else {
                result += character;
            }
        }
        throw std::runtime_error("Unterminated inventory string");
    }

    int parseInteger() {
        skipWhitespace();
        const std::size_t start = position;
        if (position < text.size() && (text[position] == '-' || text[position] == '+')) ++position;
        while (position < text.size() && std::isdigit(static_cast<unsigned char>(text[position]))) ++position;
        if (start == position || (position == start + 1 && (text[start] == '-' || text[start] == '+'))) {
            throw std::runtime_error("Expected inventory integer");
        }
        try {
            return std::stoi(text.substr(start, position - start));
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid inventory integer");
        }
    }

    std::string text;
    std::size_t position = 0;
};

std::string escapeJson(const std::string& value) {
    std::string escaped;
    for (const char character : value) {
        if (character == '"' || character == '\\') escaped += '\\';
        if (character == '\n') escaped += "\\n";
        else if (character == '\r') escaped += "\\r";
        else if (character == '\t') escaped += "\\t";
        else escaped += character;
    }
    return escaped;
}

} // namespace

InventoryManager::Product::Product(int id, std::string name, int priceCents, int quantity)
    : id(id), name(std::move(name)), priceCents(priceCents), quantity(quantity) {}
int InventoryManager::Product::getId() const { return id; }
const std::string& InventoryManager::Product::getName() const { return name; }
int InventoryManager::Product::getPriceCents() const { return priceCents; }
int InventoryManager::Product::getQuantity() const { return quantity; }

InventoryManager::InventoryManager(const std::string& filePath)
    : filePath(filePath) {
    std::ifstream inventoryFile(filePath);
    if (!inventoryFile) return;

    std::ostringstream contents;
    contents << inventoryFile.rdbuf();
    for (const auto& product : JsonParser(contents.str()).parseProducts()) {
        products.emplace(product.id, Product(product.id, product.name, product.priceCents, product.quantity));
    }
}

bool InventoryManager::createProduct(int id, const std::string& name, int priceCents, int quantity) {
    const auto [product, inserted] = products.emplace(id, Product(id, name, priceCents, quantity));
    if (!inserted || save()) return inserted;
    products.erase(product);
    return false;
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
    const Product previous = product->second;
    product->second = Product(id, name, priceCents, quantity);
    if (save()) return true;
    product->second = previous;
    return false;
}

bool InventoryManager::deleteProduct(int id) {
    const auto product = products.find(id);
    if (product == products.end()) return false;
    const Product previous = product->second;
    products.erase(product);
    if (save()) return true;
    products.emplace(id, previous);
    return false;
}

bool InventoryManager::removeQuantity(int id, int quantity) {
    const auto product = products.find(id);
    if (product == products.end() || quantity <= 0 || product->second.quantity < quantity) return false;
    product->second.quantity -= quantity;
    if (save()) return true;
    product->second.quantity += quantity;
    return false;
}

bool InventoryManager::save() const {
    std::ofstream inventoryFile(filePath);
    if (!inventoryFile) return false;

    inventoryFile << "[\n";
    bool first = true;
    for (const auto& [id, product] : products) {
        if (!first) inventoryFile << ",\n";
        first = false;
        inventoryFile << "  {\"id\": " << product.id
                      << ", \"name\": \"" << escapeJson(product.name)
                      << "\", \"priceCents\": " << product.priceCents
                      << ", \"quantity\": " << product.quantity << "}";
    }
    inventoryFile << "\n]\n";
    return static_cast<bool>(inventoryFile);
}
