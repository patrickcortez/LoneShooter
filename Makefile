CXX = g++
CXXFLAGS = -O2 -std=c++17
LDFLAGS = -lgdi32 -lwinmm -mwindows -lole32 -loleaut32 -luuid -lopengl32 -lcomctl32
SRC = src/loneshooter.cpp
OUT_DIR = bin
TARGET = $(OUT_DIR)/LoneShooter.exe
TEST_SRC = tests/test_neural.cpp
TEST_TARGET = $(OUT_DIR)/test_neural.exe
TEST_PF_SRC = tests/test_pathfinder.cpp
TEST_PF_TARGET = $(OUT_DIR)/test_pathfinder.exe

all: build

build: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(OUT_DIR) || type nul > nul
	$(CXX) -o $@ $^ $(LDFLAGS) $(CXXFLAGS)

test: $(TEST_TARGET) $(TEST_PF_TARGET)
	$(TEST_TARGET)
	$(TEST_PF_TARGET)

$(TEST_TARGET): $(TEST_SRC) src/neural.hpp
	@mkdir -p $(OUT_DIR) || type nul > nul
	$(CXX) -o $@ $(TEST_SRC) $(CXXFLAGS) -I src -lgdi32

$(TEST_PF_TARGET): $(TEST_PF_SRC) src/pathfinder.hpp
	@mkdir -p $(OUT_DIR) || type nul > nul
	$(CXX) -o $@ $(TEST_PF_SRC) $(CXXFLAGS) -I src -lgdi32

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(TEST_PF_TARGET)
