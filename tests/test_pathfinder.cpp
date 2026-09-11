// tests/test_pathfinder.cpp
#include <cstdio>
#include <vector>
#include <cassert>
#include "pathfinder.hpp"

static int g_tests  = 0;
static int g_passed = 0;

#define CHECK(cond, msg) do {                                   \
    ++g_tests;                                                  \
    if (cond) { ++g_passed; printf("  PASS: %s\n", msg); }     \
    else       { printf("  FAIL: %s  (line %d)\n", msg, __LINE__); } \
} while(0)

int testMap[64][64];

bool DummyCollision(float x, float y, float radius) {
    return false;
}

void test_basic_path() {
    printf("Test 1: Basic Path\n");
    for(int i=0; i<64; i++) for(int j=0; j<64; j++) testMap[i][j] = 0;
    
    Pathfinder::Init(testMap, DummyCollision);
    auto path = Pathfinder::FindPath(1.0f, 1.0f, 5.0f, 5.0f);
    CHECK(path.size() > 0, "Finds a path in open map");
    CHECK(path.back().first == 5 && path.back().second == 5, "Path reaches target");
}

void test_blocked_path() {
    printf("Test 2: Blocked Path / Partial Fallback\n");
    // Block the target completely via a wall across the map
    for(int i=0; i<64; i++) testMap[i][4] = 1; 
    
    auto path = Pathfinder::FindPath(1.0f, 1.0f, 5.0f, 5.0f);
    CHECK(path.size() > 0, "Returns partial path due to fallback");
    CHECK(path.back().second < 4, "Path stops before the wall");
}

int main() {
    printf("=== Pathfinder Unit Tests ===\n\n");
    test_basic_path();
    test_blocked_path();
    printf("\n%d / %d tests passed.\n", g_passed, g_tests);
    return (g_passed == g_tests) ? 0 : 1;
}
