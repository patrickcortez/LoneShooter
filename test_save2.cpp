#include <iostream>
#include <stdio.h>

struct Player {
    float x, y;
    float angle;
    float pitch;
    int health;
    int maxHealth = 100;
    int level = 1;
    int xp = 0;
    int xpToNextLevel = 100;
};

int main() {
    FILE* f = _wfopen(L"configs/savegame.dat", L"wb");
    Player p;
    p.health = 100;
    int score = 42;
    fwrite(&p, sizeof(Player), 1, f);
    fwrite(&score, sizeof(int), 1, f);
    fclose(f);

    f = _wfopen(L"configs/savegame.dat", L"rb");
    Player p2;
    int score2 = 0;
    fread(&p2, sizeof(Player), 1, f);
    fread(&score2, sizeof(int), 1, f);
    std::cout << "Score read: " << score2 << "\n";
    fclose(f);
}
