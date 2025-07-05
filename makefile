PROJECT          := eml
SOURCE_DIRECTORY := src
MISC             := $(SOURCE_DIRECTORY)/misc
TEST_DIR         := tests
ENTRY            := $(SOURCE_DIRECTORY)/main.cpp
CSRC             := $(shell find $(SOURCE_DIRECTORY) -type f -name "*.c")
CPPSRC           := $(wildcard $(MISC)/*.cpp $(SOURCE_DIRECTORY)/*.cpp)
SRC              := $(CSRC) $(CPPSRC)
OUT_DIRECTORY    := bin
TARGET           := $(OUT_DIRECTORY)/$(PROJECT)
OBJ_NAMES        := $(notdir $(SRC))
OBJ              := $(addprefix bin/, $(OBJ_NAMES:.c=.o))
OBJ              := $(OBJ:.cpp=.o)

MOBJ             := $(patsubst $(SOURCE_DIRECTORY)/%.c, $(OUT_DIRECTORY)/%.o, $(CPPSRC))
FLAGS            := -g -std=c++20

define MAP_OBJ_TO_SRC $(foreach src, $(SRC), $(eval SRCS_by_OBJ_bin/$(notdir $(src:.c=.o)) := $(src)) $(eval SRCS_by_OBJ_bin/$(notdir $(src:.cpp=.o)) := $(src)))
endef
$(MAP_OBJ_TO_SRC)


TEST_FILES      := $(wildcard $(TEST_DIR)/*.eml)

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "\033[33mBuilding $@\033[0m"
	@$(CXX) -o $@ $(OBJ) $(FLAGS)

$(OUT_DIRECTORY)/main.o:
	@echo "\033[33mBuilding $@\033[0m"
	@$(CC) -c $(ENTRY) -o $@ $(FLAGS)

$(OUT_DIRECTORY)/%.o:
	@if echo $(SRCS_by_OBJ_$@) | grep -q '\.c$$'; then \
	echo "\033[33mBuilding $(SRCS_by_OBJ_$@) -> $@\033[0m"; \
	$(CC) -c -o $@ $(SRCS_by_OBJ_$@) $(FLAGS); \
	else \
	echo "\033[33mBuilding $(SRCS_by_OBJ_$@) -> $@\033[0m"; \
	$(CXX) -c -o $@ $(SRCS_by_OBJ_$@) $(FLAGS); \
	fi

TEST_OUTPUTS := $(TEST_FILES:.eml=.test)

test: $(TEST_OUTPUTS)
	@echo "Done testing $(TEST_FILES)"

%.test: %.eml
	@echo "-- \033[33mTesting $<\033[0m"
	@./$(TARGET) -i $<
