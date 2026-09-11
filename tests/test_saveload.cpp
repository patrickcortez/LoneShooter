// tests/test_saveload.cpp
// Unit tests for verifying the fallback loading logic of savegame.dat

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

// -----------------------------------------------------------------
// Minimal test harness
// -----------------------------------------------------------------
static int g_tests  = 0;
static int g_passed = 0;

#define CHECK(cond, msg) do {                                   \
    ++g_tests;                                                  \
    if (cond) { ++g_passed; printf("  PASS: %s\n", msg); }     \
    else       { printf("  FAIL: %s  (line %d)\n", msg, __LINE__); } \
} while(0)

// Mock the exact variables used in the game's SaveGame / LoadGame
struct PlayerMock {
    float x, y, angle, pitch;
    int health, maxHealth, level, xp, xpToNextLevel;
};

// Global mocks
PlayerMock player = {10.0f, 32.0f, 0.0f, 0.0f, 100, 100, 1, 0, 100};
int score = 0;
bool bossActive = false;
int currentWeapon = 0;

// The appended new variables
int playerDamage = 1;
float g_BonusSpeed = 0.0f;
int g_PendingUpgrades = 0;
bool bossDead = false;

// Mock Save function mimicking an OLD save file (without appended variables)
void CreateOldSaveFile(const wchar_t* path) {
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    
    fwrite(&player, sizeof(PlayerMock), 1, f);
    fwrite(&score, sizeof(int), 1, f);
    fwrite(&bossActive, sizeof(bool), 1, f);
    fwrite(&currentWeapon, sizeof(int), 1, f);
    // Did NOT save the newly appended fields
    
    fclose(f);
}

// Mock Save function mimicking a NEW save file
void CreateNewSaveFile(const wchar_t* path) {
    FILE* f = _wfopen(path, L"wb");
    if (!f) return;
    
    fwrite(&player, sizeof(PlayerMock), 1, f);
    fwrite(&score, sizeof(int), 1, f);
    fwrite(&bossActive, sizeof(bool), 1, f);
    fwrite(&currentWeapon, sizeof(int), 1, f);
    
    // New fields
    fwrite(&playerDamage, sizeof(int), 1, f);
    fwrite(&g_BonusSpeed, sizeof(float), 1, f);
    fwrite(&g_PendingUpgrades, sizeof(int), 1, f);
    fwrite(&bossDead, sizeof(bool), 1, f);
    
    fclose(f);
}

// Mock Load function mimicking the updated game logic
bool LoadGameMock(const wchar_t* path) {
    FILE* f = _wfopen(path, L"rb");
    if (!f) return false;
    
    try {
        fread(&player, sizeof(PlayerMock), 1, f);
        fread(&score, sizeof(int), 1, f);
        fread(&bossActive, sizeof(bool), 1, f);
        fread(&currentWeapon, sizeof(int), 1, f);
        
        // Read appended player variables (with backward compatibility)
        if (fread(&playerDamage, sizeof(int), 1, f) != 1) playerDamage = 1;
        if (fread(&g_BonusSpeed, sizeof(float), 1, f) != 1) g_BonusSpeed = 0.0f;
        if (fread(&g_PendingUpgrades, sizeof(int), 1, f) != 1) g_PendingUpgrades = 0;
        
        if (fread(&bossDead, sizeof(bool), 1, f) != 1) bossDead = false;
        
    } catch (...) {
        fclose(f);
        return false;
    }
    
    fclose(f);
    return true;
}

void test_load_old_save() {
    printf("Test: Load an older save file missing the new fields\n");
    
    const wchar_t* tmpPath = L"test_oldsave.dat";
    CreateOldSaveFile(tmpPath);
    
    // Mess up the variables to ensure defaults are applied
    playerDamage = 999;
    g_BonusSpeed = 999.0f;
    g_PendingUpgrades = 999;
    bossDead = true;
    
    bool result = LoadGameMock(tmpPath);
    CHECK(result, "LoadGame completes successfully");
    
    CHECK(playerDamage == 1, "playerDamage defaults correctly");
    CHECK(g_BonusSpeed == 0.0f, "g_BonusSpeed defaults correctly");
    CHECK(g_PendingUpgrades == 0, "g_PendingUpgrades defaults correctly");
    CHECK(bossDead == false, "bossDead defaults correctly");
    
    _wremove(tmpPath);
}

void test_load_new_save() {
    printf("Test: Load a new save file with appended fields\n");
    
    playerDamage = 5;
    g_BonusSpeed = 2.5f;
    g_PendingUpgrades = 3;
    bossDead = true;
    
    const wchar_t* tmpPath = L"test_newsave.dat";
    CreateNewSaveFile(tmpPath);
    
    // Mess up the variables
    playerDamage = 0;
    g_BonusSpeed = 0.0f;
    g_PendingUpgrades = 0;
    bossDead = false;
    
    bool result = LoadGameMock(tmpPath);
    CHECK(result, "LoadGame completes successfully");
    
    CHECK(playerDamage == 5, "playerDamage read correctly");
    CHECK(g_BonusSpeed == 2.5f, "g_BonusSpeed read correctly");
    CHECK(g_PendingUpgrades == 3, "g_PendingUpgrades read correctly");
    CHECK(bossDead == true, "bossDead read correctly");
    
    _wremove(tmpPath);
}

int main() {
    printf("=== Save/Load Unit Tests ===\n\n");

    test_load_old_save();
    test_load_new_save();

    printf("\n%d / %d tests passed.\n", g_passed, g_tests);
    return (g_passed == g_tests) ? 0 : 1;
}
