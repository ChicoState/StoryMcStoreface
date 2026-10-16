CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

TARGET ?= storymcstoreface
SOURCES := $(filter-out PaymentTypesTests.cpp,$(wildcard *.cpp))
OBJECTS := $(SOURCES:.cpp=.o)

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


clean:
	$(RM) $(TARGET) $(TEST_TARGET) $(OBJECTS) $(OBJECTS:.o=.d)
