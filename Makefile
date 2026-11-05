CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wno-switch -O2
CPPFLAGS := -Isrc/lexer -Isrc/parser -Isrc/parser/ASTNodes
TARGET   := compiler.exe
TEST_BIN := tests.exe

LIB_SRC := src/lexer/lexer.cpp \
           src/parser/tokenParser.cpp \
           src/parser/ASTNodes/tokenVector.cpp \
           src/parser/ASTNodes/BoolExprNodes.cpp \
           src/parser/ASTNodes/ExprNodes.cpp \
           src/parser/ASTNodes/StatementNodes.cpp \
           src/parser/ASTNodes/TopLevelNodes.cpp \
           src/parser/ASTNodes/TypeNodes.cpp

MAIN_SRC := src/main.cpp
TEST_SRC := tests/parserTests.cpp

LIB_OBJ  := $(LIB_SRC:src/%.cpp=build/%.o)
MAIN_OBJ := $(MAIN_SRC:src/%.cpp=build/%.o)
TEST_OBJ := $(TEST_SRC:tests/%.cpp=build/tests/%.o)

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIB_OBJ)
	$(CXX) $^ -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIB_OBJ)
	$(CXX) $^ -o $@

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

build/tests/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -Isrc -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf build $(TARGET) $(TEST_BIN)

.PHONY: all clean test