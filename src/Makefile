PROJECT_NAME := tetris
UNAME_S = $(shell uname -s)

CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c11

LOGIC_DIR := ./brick_game/tetris
GUI_DIR := ./gui/cli
HEADER_DIR := ./include
TEST_DIR := ./tests

LOGIC_SRC := $(wildcard $(LOGIC_DIR)/*.c)
GUI_SRC := $(wildcard $(GUI_DIR)/*.c)
ALL_SRC := $(LOGIC_SRC) $(GUI_SRC)
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)

LOGIC_OBJ := $(LOGIC_SRC:.c=.o)
GUI_OBJ := $(GUI_SRC:.c=.o)
ALL_OBJ := $(LOGIC_OBJ) $(GUI_OBJ)

LOGIC_SRC_WITHOUT_MAIN := $(filter-out $(LOGIC_DIR)/tetris.c, $(LOGIC_SRC))
LOGIC_TEST_OBJ := $(LOGIC_SRC_WITHOUT_MAIN:.c=.test.o)
TEST_OBJ := $(TEST_SRC:.c=.test.o) 
ALL_TEST_OBJ := $(LOGIC_TEST_OBJ) $(TEST_OBJ)

HEADERS := $(wildcard $(HEADER_DIR)/*.h)

DOCS_DIR := ./docs
BUILD_DIR := ./build
OUTPUT_DIR := ./out
REPORT_DIR := ./report
GCOV_DIR := ./gcov_report

EXEC_FILENAME := $(PROJECT_NAME)_bin
DOC_FILENAME := $(PROJECT_NAME)_doc
DIST_FILENAME := $(PROJECT_NAME)_dist
TEST_BIN_FILENAME := $(PROJECT_NAME)_test_bin

ifeq ($(UNAME_S),Linux)
	CC+= -DLINUX
	FLAGS_TEST = -lcheck -lsubunit -lm
	IGNORE_LCOV_FLAGS :=
    CMD_OPEN = xdg-open
else ifeq ($(UNAME_S),Darwin)
	CC+= -DDARWIN
	FLAGS_TEST = -lcheck
	IGNORE_LCOV_FLAGS := --ignore-errors unsupported,unsupported
	CMD_OPEN = open
endif

.PHONY: all install uninstall clean dvi dist test test-bin gcov_report \
valgrind gen-dockerfile image shell clean-docker

all: install dvi dist test gcov_report 

install: $(ALL_OBJ)
	@mkdir -p $(OUTPUT_DIR)
	@$(CC) $(CFLAGS) $^ -lncurses -o $(OUTPUT_DIR)/$(EXEC_FILENAME)

$(LOGIC_DIR)/%.o: $(LOGIC_DIR)/%.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

$(GUI_DIR)/%.o: $(GUI_DIR)/%.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

uninstall:
	@rm -rf $(OUTPUT_DIR)

run: install
	@$(OUTPUT_DIR)/$(EXEC_FILENAME)

dist:
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)
	@tar -czvf $(BUILD_DIR)/$(DIST_FILENAME).tar.gz \
	./Makefile ./Doxyfile brick_game/* gui/* include/* tests/* || true

clean: uninstall clean-report
	@rm -rf $(ALL_OBJ)
	@rm -rf $(ALL_TEST_OBJ)
	@rm -rf $(TEST_BIN_FILENAME)
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DOCS_DIR) $(DOXYFILE)
	@rm -rf ./valgrind_report.txt

rebuild: clean all

# ------------------------------
# UNIT TESTS AND COVERAGE
# ------------------------------

test: $(TEST_BIN_FILENAME)
	@./$(TEST_BIN_FILENAME)
	@rm -f $(OUTPUT_DIR)/record_note.txt

$(TEST_BIN_FILENAME): $(ALL_TEST_OBJ)
	@$(CC) $(CFLAGS) -DUSE_MOCK $(ALL_TEST_OBJ) $(FLAGS_TEST) -o $@

%.test.o: %.c $(HEADERS)
	@$(CC) $(CFLAGS) -DUSE_MOCK -c $< -o $@

gcov_report: clean-report $(LOGIC_SRC_WITHOUT_MAIN) $(TEST_SRC) $(HEADERS)
	@$(CC) $(CFLAGS) -DUSE_MOCK --coverage -c \
		$(LOGIC_SRC_WITHOUT_MAIN) $(TEST_SRC)
	@mkdir -p $(GCOV_DIR)
	@$(CC) $(CFLAGS) --coverage *.o $(FLAGS_TEST) -o $(GCOV_DIR)/$@
	@mkdir -p $(OUTPUT_DIR)
	@echo '100' >> $(OUTPUT_DIR)/record_note.txt
	-@$(GCOV_DIR)/$@ > /dev/null 2>&1
	@mkdir -p $(REPORT_DIR)
	@lcov $(IGNORE_LCOV_FLAGS) -q -t $@ --output-file $@.info \
	--capture --directory . --exclude '*/tests*'
	@genhtml $@.info -o report
	@echo "HTML report generated"
	@rm -rf *.o *.gcno *.gcda *.gcov $@.info $(OUTPUT_DIR)/record_note.txt

clean-report:
	@rm -rf $(GCOV_DIR)
	@rm -rf $(REPORT_DIR)
	@rm -rf *.gcno *.gcda *.gcov *.info *.o

open-report: $(GCOV_DIR)/gcov_report
	@$(CMD_OPEN) "$(REPORT_DIR)/index.html"

# ------------------------------
# DOCUMENTATION
# ------------------------------

DOXYFILE := Doxyfile

define DOXYFILE_TEMPLATE
DOXYFILE_ENCODING      = UTF-8
PROJECT_NAME           = "$(PROJECT_NAME) gillyhol"
PROJECT_NUMBER         = 1.0
PROJECT_BRIEF          = $(PROJECT_NAME) documentation
INPUT                  = $(LOGIC_DIR) $(GUI_DIR) $(HEADER_DIR) $(TEST_DIR)
OUTPUT_DIRECTORY       = $(DOCS_DIR)
GENERATE_LATEX         = YES
FILE_PATTERNS          = *.c *.h
RECURSIVE              = YES
QUIET                  = YES
LATEX_BATCHMODE        = YES
WARNINGS               = NO
EXTRACT_ALL            = YES
EXTRACT_PRIVATE.       = YES
EXTRACT_STATIC         = YES
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
TYPEDEF_HIDES_STRUCT   = NO
endef

export DOXYFILE_TEMPLATE

dvi: $(ALL_SRC) $(HEADERS) $(TEST_SRC)
	@if [ ! -f $(DOXYFILE) ]; then \
		echo "$$DOXYFILE_TEMPLATE" > $(DOXYFILE); \
	fi
	@rm -rf $(DOCS_DIR)
	@mkdir -p $(DOCS_DIR)
	@doxygen $(DOXYFILE) 2>/dev/null || true
	@cd $(DOCS_DIR)/latex && make LATEX_QUIET=YES 2>/dev/null || true

# 	mistake in tex lib, need to be changed manually
ifeq ($(UNAME_S),Darwin)
	@cd $(DOCS_DIR)/latex && \
	@sed -i '' 's/\\usepackage\[pdftex,pagebackref=true\]{hyperref}/ \
	\\usepackage[dvips,pagebackref=true]{hyperref}/g' \
	refman.tex 2>/dev/null || true
endif

	@cd $(DOCS_DIR)/latex && latex -interaction=batchmode refman.tex 2>/dev/null || true
	@mv $(DOCS_DIR)/latex/refman.dvi $(DOCS_DIR)/$(DOC_FILENAME).dvi || true
	@rm -rf $(DOCS_DIR)/latex

open-dvi: $(DOCS_DIR)/html/index.html
	open $(DOCS_DIR)/html/index.html

open-pic:
	open ../misc/FSM_tetris_html/index.html

open-visual: open-report open-dvi open-pic

# ------------------------------
# CHECKERS
# ------------------------------

cpp:
	cppcheck --enable=all --suppress=missingIncludeSystem $(ALL_SRC) $(TEST_SRC) $(HEADERS)

clang-test:
	@clang-format --style=Google -n $(ALL_SRC) $(TEST_SRC) $(HEADERS)

clang:
	@clang-format --style=Google -i $(ALL_SRC) $(TEST_SRC) $(HEADERS)

# ------------------------------
# DOCKER CONTAINER FOR MAC
# ------------------------------

DOCKERFILE := Dockerfile
IMAGE_NAME := valgrind-c

define DOCKER_TEMPLATE
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y build-essential gcc make valgrind cppcheck \
	check libsubunit-dev lcov clang-format git libncurses5-dev libncursesw5-dev \
	doxygen doxygen-latex \
    && rm -rf /var/lib/apt/lists/* 

WORKDIR /app
endef

export DOCKER_TEMPLATE

gen-dockerfile:
	@echo "$$DOCKER_TEMPLATE" > $(DOCKERFILE)

image: gen-dockerfile
	@docker build -t $(IMAGE_NAME) .

shell: image
	@docker run -it --rm -v "$(CURDIR)":/app $(IMAGE_NAME) /bin/bash

clean-docker:
	@rm -f $(DOCKERFILE) valgrind_report.txt
	@docker rmi $(IMAGE_NAME) 2>/dev/null || true

clean-all: clean clean-docker

# ------------------------------
# VALGRIND IN LINUX CONTAINER
# ------------------------------

valgrind: install test
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--error-exitcode=1 $(OUTPUT_DIR)/$(EXEC_FILENAME) ./$(TEST_BIN_FILENAME)

valgrind-verbose: install test
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--verbose --error-exitcode=1 $(OUTPUT_DIR)/$(EXEC_FILENAME) ./$(TEST_BIN_FILENAME)

valgrind-log: install test
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--log-file=valgrind_report.txt $(OUTPUT_DIR)/$(EXEC_FILENAME) ./$(TEST_BIN_FILENAME)

valgrind-quick-run: install
	@valgrind --leak-check=yes --error-exitcode=1 \
	$(OUTPUT_DIR)/$(EXEC_FILENAME)

valgrind-quick-test: test
	@valgrind --leak-check=yes --error-exitcode=1 \
	./$(TEST_BIN_FILENAME)

valgrind-quick: install test
	@valgrind --leak-check=yes --error-exitcode=1 \
	$(OUTPUT_DIR)/$(EXEC_FILENAME) ./$(TEST_BIN_FILENAME)
