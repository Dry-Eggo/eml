


# === Project Name ===
# The final binary will be named "eml"
PROJECT := eml

# === Source & Output Directories ===
# SRC_DIR contains all your .c/.cpp source code
# OUT_DIR is where .o files and the binary go
# TEST_DIR is where .eml test scripts live
SRC_DIR := src
OUT_DIR := bin
TEST_DIR := tests

# === Entry Point ===
# This is the main.cpp file used as the starting point
ENTRY := $(SRC_DIR)/main.cpp

# === Source File Discovery ===
# These find all .c and .cpp files in the source dir (recursively)
C_SOURCES   := $(shell find $(SRC_DIR) -type f -name "*.c")
CPP_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
SOURCES     := $(C_SOURCES) $(CPP_SOURCES)

# === Object Files ===
# These lines convert your .cpp/.c files into .o files in OUT_DIR
# For example: src/foo.cpp -> bin/foo.o
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OUT_DIR)/%.o, $(CPP_SOURCES))
OBJECTS += $(patsubst $(SRC_DIR)/%.c,   $(OUT_DIR)/%.o, $(C_SOURCES))

# === Final Binary ===
# The built executable will be at bin/eml
TARGET := $(OUT_DIR)/$(PROJECT)

# === Compiler Flags ===
# -g adds debug info, -std=c++20 is for modern C++ features (like std::format!)
FLAGS := -std=c++20 -g -O1

# === Default Rule ===
# This runs when you just run `make`
# Depends on the target binary (which in turn depends on object files)
all: $(TARGET)

# === Link Rule ===
# When all object files are compiled, link them into the final binary
$(TARGET): $(OBJECTS)
	@echo "\033[32mLinking $@\033[0m"
	@$(CXX) -o $@ $^ $(FLAGS)
	@sudo cp $(TARGET) /usr/bin/$(PROJECT)

# === Compilation Rules for C++ ===
# Turns each src/foo.cpp into bin/foo.o
# $(dir $@) ensures the `bin/` folder exists before compiling
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "\033[33mCompiling $< -> $@\033[0m"
	@$(CXX) -c $< -o $@ $(FLAGS)

# === Compilation Rules for C ===
# Same idea, but uses $(CC) instead of $(CXX)
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "\033[33mCompiling $< -> $@\033[0m"
	@$(CC) -c $< -o $@ $(FLAGS)

# === Test Discovery ===
# Grab all test files in the test dir that end in .eml
TEST_FILES := $(wildcard $(TEST_DIR)/*.eml)

# === Map test files to fake targets ===
# Each test.eml will generate test.test (but not actually write a file)
TEST_OUTPUTS := $(TEST_FILES:.eml=.test)

# === Test Rule ===
# Running `make test` will build everything first,
# then run the test command on each test file
test: $(TEST_OUTPUTS)
	@echo "\033[32mAll tests done.\033[0m"

# === Per-Test Rule ===
# For each test.eml, run it with your interpreter
# The %.test target is fake—its job is to run the .eml file
%.test: %.eml $(TARGET)
	@echo "-- \033[33mTesting $<\033[0m"
	@./$(TARGET) -i $<

# === Clean Rule ===
# `make clean` deletes all built files
clean:
	rm -rf $(OUT_DIR)/*.o $(TARGET) $(TEST_DIR)/*.test
