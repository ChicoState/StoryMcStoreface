CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

TARGET ?= storymcstoreface
SOURCES := $(filter-out PaymentTypesTests.cpp,$(wildcard *.cpp))
OBJECTS := $(SOURCES:.cpp=.o)
TEST_TARGET ?= payment_tests
TEST_SOURCES := PaymentTypesTests.cpp PaymentTypes.cpp

.PHONY: all build run test clean

all: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(TARGET) $(TEST_TARGET) $(OBJECTS) $(OBJECTS:.o=.d)
