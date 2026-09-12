#include <iostream>
#include <stdio.h>

struct Player {
    float x, y;
    float angle;
    float pitch;
    int health;
    int maxHealth;
    int level;
    int xp;
    int xpToNextLevel;
};

int main() {
    FILE* f = _wfopen(L"configs/savegame.dat", L"rb");
    if(!f) { std::cout << "No save\n"; return 0; }
    Player p;
    fread(&p, sizeof(Player), 1, f);
    int score;
    fread(&score, sizeof(int), 1, f);
    std::cout << "Score in save: " << score << "\n";
    fclose(f);
}
