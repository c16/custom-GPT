# Makefile for Claude Agent C++ (Gtkmm version)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Iinclude
LIBS = `pkg-config --libs gtkmm-3.0`
CFLAGS = `pkg-config --cflags gtkmm-3.0`

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Target executable
TARGET = $(BINDIR)/ClaudeAgentGtk

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Link target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential pkg-config libgtkmm-3.0-dev cmake

# Run the application
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: all

# Check if dependencies are available
check-deps:
	@echo "Checking for gtkmm-3.0..."
	@pkg-config --exists gtkmm-3.0 && echo "gtkmm-3.0: OK" || echo "gtkmm-3.0: NOT FOUND"
	@echo "Checking for C++ compiler..."
	@which $(CXX) > /dev/null && echo "$(CXX): OK" || echo "$(CXX): NOT FOUND"

.PHONY: all clean install-deps run debug check-deps directories