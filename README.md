# StoryMcStoreface

StoryMcStoreface is a small C++ console application for viewing inventory and placing simulated orders. Payments are simulated locally; no real payment is processed.

## Getting Started

### Prerequisites

- A C++ compiler with C++17 support
- GNU Make

This project is compatible with [cpp-container](https://github.com/ChicoState/cpp-container) docker container. If you already have the container built, just run:

```sh
docker run -v "$(pwd)":/usr/src -it cpp-container
```
And that will build and run the application.

### Manual Build

From the project directory, run:

```sh
make
```

This compiles the application with C++17 and creates the `storymcstoreface` executable.

### Manual Run

Start the application with:

```sh
make run
```

The menu allows you to view inventory, place an order, or exit. When an order is placed, one or more simulated payment methods can be selected. Store credit can cover part of an order, with the remaining amount charged to another selected method.

###  Manually Clean Build Artifacts

To remove the executable, object files, and dependency files:

```sh
make clean
```

## Build Targets

| Command | Description |
| --- | --- |
| `make` or `make build` | Build the application |
| `make run` | Build, then launch the application |
| `make clean` | Remove generated build artifacts |
| `./test_runner.sh` | Build and run the interactive program |

## Roadmap

The current class relationships are represented below. The diagram also highlights the direct dependencies that are candidates for future decoupling.

```mermaid
classDiagram
    class InventoryManager {
        +Product products[5]
        +int productCount
        +InventoryManager()
    }

    class Product {
        +int id
        +string name
        +int priceCents
        +int quantity
    }

    class OrderProcessor {
        -InventoryManager inventory
        -int nextOrderNumber
        +OrderProcessor(InventoryManager inventory)
        +bool placeOrder(int productId, int quantity, string customerName, string customerEmail, vector~PaymentSelection~ payments)
    }

    class IPaymentType {
        +int charge(int totalCents, string customerId, string authentication)
    }

    class CreditCard
    class CouponCode
    class StoreCredit

    class PaymentSelection {
        +PaymentMethod method
        +string authentication
        +int availableCents
    }

    class NotificationService {
        +void sendEmail(string recipient, string subject, string message)
    }

    InventoryManager *-- Product : stores
    OrderProcessor --> InventoryManager : reads and updates
    IPaymentType <|.. CreditCard
    IPaymentType <|.. CouponCode
    IPaymentType <|.. StoreCredit
    OrderProcessor ..> IPaymentType : creates
    OrderProcessor ..> NotificationService : creates
```

Planned improvements include encapsulating inventory storage and isolating notification delivery behind an interface.
