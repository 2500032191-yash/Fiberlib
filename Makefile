CC ?= gcc
CFLAGS ?= -Wall -Wextra -Iinclude -g -O2 -D_GNU_SOURCE -Wno-deprecated-declarations
AR ?= ar
ARFLAGS = rcs

LIB_DIR = lib
BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
EXAMPLES_DIR = examples
TESTS_DIR = tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRCS))
STATIC_LIB = $(LIB_DIR)/libfiber.a

EXAMPLE_SRCS = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c, $(BIN_DIR)/%, $(EXAMPLE_SRCS))

TEST_SRCS = $(wildcard $(TESTS_DIR)/*.c)
TEST_BINS = $(patsubst $(TESTS_DIR)/%.c, $(BIN_DIR)/%, $(TEST_SRCS))

.PHONY: all clean test run-examples directories

all: directories $(STATIC_LIB) $(EXAMPLE_BINS) $(TEST_BINS)

directories:
	@mkdir -p $(LIB_DIR) $(BIN_DIR)

$(STATIC_LIB): $(OBJS) | directories
	$(AR) $(ARFLAGS) $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.c $(STATIC_LIB) | directories
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lfiber -o $@

$(BIN_DIR)/test_%: $(TESTS_DIR)/test_%.c $(STATIC_LIB) | directories
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lfiber -o $@

test: $(TEST_BINS)
	@echo "========================================="
	@echo " RUNNING FIBERLIB UNIT TESTS"
	@echo "========================================="
	@for t in $(TEST_BINS); do \
		echo "Running $$t..."; \
		$$t || exit 1; \
		echo ""; \
	done
	@echo "ALL UNIT TESTS PASSED SUCCESSFULLY!"

run-examples: $(EXAMPLE_BINS)
	@echo "========================================="
	@echo " RUNNING ALL 8 FIBERLIB DEMOS"
	@echo "========================================="
	@for demo in $(EXAMPLE_BINS); do \
		echo ">>> Running $$demo <<<"; \
		$$demo; \
		echo ""; \
	done

clean:
	rm -rf $(SRC_DIR)/*.o $(LIB_DIR) $(BIN_DIR) fiber_io_test.tmp
