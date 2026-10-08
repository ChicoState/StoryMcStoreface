CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic

TARGET ?= storymcstoreface
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all build run clean

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
	$(RM) $(TARGET) $(OBJECTS) $(OBJECTS:.o=.d)
