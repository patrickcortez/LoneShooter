CXX = g++
WINDRES = windres
CXXFLAGS = -O2 -std=c++17 -static -static-libgcc -static-libstdc++
LDFLAGS = -lgdi32 -lwinmm -mwindows -lole32 -loleaut32 -luuid -lopengl32 -lcomctl32
SRC = src/loneshooter.cpp
RC_SRC = src/resource.rc
RES_OBJ = src/resource.res
OUT_DIR = bin
TARGET = $(OUT_DIR)/LoneShooter.exe
TEST_SRC = tests/test_neural.cpp
TEST_TARGET = $(OUT_DIR)/test_neural.exe
TEST_PF_SRC = tests/test_pathfinder.cpp
TEST_PF_TARGET = $(OUT_DIR)/test_pathfinder.exe

all: build

build: $(TARGET)

$(RES_OBJ): $(RC_SRC)
	$(WINDRES) $(RC_SRC) -O coff -o $(RES_OBJ)

$(TARGET): $(SRC) $(RES_OBJ)
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
	rm -f $(TARGET) $(TEST_TARGET) $(TEST_PF_TARGET) $(RES_OBJ)

deploy: build
	@powershell -Command "if (Get-Command iscc -ErrorAction SilentlyContinue) { iscc installer.iss } elseif (Test-Path 'C:\Program Files (x86)\Inno Setup 6\ISCC.exe') { & 'C:\Program Files (x86)\Inno Setup 6\ISCC.exe' installer.iss } else { Write-Host 'Error: Inno Setup not found. Run winget install -e --id JRSoftware.InnoSetup'; exit 1 }"
