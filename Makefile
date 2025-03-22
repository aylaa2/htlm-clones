# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 $(shell pkg-config --cflags gumbo)
LDFLAGS = $(shell pkg-config --libs gumbo)

# Target executable
TARGET = main

# Source files
SRCS = main.cpp

# Default rule
all: $(TARGET)

# Compile the program
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean rule to remove compiled files
clean:
	rm -f $(TARGET)
