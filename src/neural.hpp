// neural.hpp - Neuroevolution AI for LoneShooter
// Include after Enemy struct is declared
// Enemies learn to counter player over generations

#ifndef NEURAL_HPP
#define NEURAL_HPP

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cwchar>
#include <windows.h>
#include <random>
#include <stdexcept>
#include <algorithm>

namespace NeuralAI {

// ---------------------------------------------------------------------------
// Network topology
// ---------------------------------------------------------------------------
const int   INPUT_COUNT      = 13;  // +bullet danger, +wall, +generation, +sin/cos angle
const int   HIDDEN_COUNT     = 10;  // Increased capacity
const int   OUTPUT_COUNT     = 5;   // +dodgeBoost, +coverSeek
const float MUTATION_RATE    = 0.15f;
const float MUTATION_STRENGTH = 0.3f;
const float WEIGHT_CLAMP     = 2.0f;
const int   POOL_SIZE        = 5;

static std::mt19937 rng(1337);

inline float RandomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

inline int RandomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

inline void Softmax(const float* inputs, float* outputs, int count) {
    float maxVal = inputs[0];
    for (int i = 1; i < count; i++) {
        if (inputs[i] > maxVal) maxVal = inputs[i];
    }
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        outputs[i] = expf(inputs[i] - maxVal);
        sum += outputs[i];
    }
    for (int i = 0; i < count; i++) {
        outputs[i] /= sum;
    }
}

// ---------------------------------------------------------------------------
// NeuralNet
// ---------------------------------------------------------------------------
struct NeuralNet {
    float weightsIH[INPUT_COUNT][HIDDEN_COUNT];
    float weightsHO[HIDDEN_COUNT][OUTPUT_COUNT];
    float biasH[HIDDEN_COUNT];
    float biasO[OUTPUT_COUNT];

    float fitness;
    float survivalTime;
    float damageDealt;
    int   bulletsEvaded; // Incremented when a dodge successfully avoids a bullet

    void Randomize() {
        for (int i = 0; i < INPUT_COUNT; i++)
            for (int h = 0; h < HIDDEN_COUNT; h++)
                weightsIH[i][h] = RandomFloat(-1.0f, 1.0f);

        for (int h = 0; h < HIDDEN_COUNT; h++) {
            for (int o = 0; o < OUTPUT_COUNT; o++)
                weightsHO[h][o] = RandomFloat(-1.0f, 1.0f);
            biasH[h] = RandomFloat(-0.5f, 0.5f);
        }
        for (int o = 0; o < OUTPUT_COUNT; o++)
            biasO[o] = RandomFloat(-0.5f, 0.5f);

        fitness       = 0.0f;
        survivalTime  = 0.0f;
        damageDealt   = 0.0f;
        bulletsEvaded = 0;
    }

    void CopyFrom(const NeuralNet& other) {
        memcpy(weightsIH, other.weightsIH, sizeof(weightsIH));
        memcpy(weightsHO, other.weightsHO, sizeof(weightsHO));
        memcpy(biasH,     other.biasH,     sizeof(biasH));
        memcpy(biasO,     other.biasO,     sizeof(biasO));
        fitness       = 0.0f;
        survivalTime  = 0.0f;
        damageDealt   = 0.0f;
        bulletsEvaded = 0;
    }

    // mutationScale > 1.0 increases exploration (used in early generations)
    void Mutate(float mutationScale = 1.0f) {
        const float strength = MUTATION_STRENGTH * mutationScale;

        for (int i = 0; i < INPUT_COUNT; i++) {
            for (int h = 0; h < HIDDEN_COUNT; h++) {
                if (RandomFloat(0.0f, 1.0f) < MUTATION_RATE) {
                    weightsIH[i][h] += RandomFloat(-1.0f, 1.0f) * strength;
                    if (weightsIH[i][h] >  WEIGHT_CLAMP) weightsIH[i][h] =  WEIGHT_CLAMP;
                    if (weightsIH[i][h] < -WEIGHT_CLAMP) weightsIH[i][h] = -WEIGHT_CLAMP;
                }
            }
        }

        for (int h = 0; h < HIDDEN_COUNT; h++) {
            for (int o = 0; o < OUTPUT_COUNT; o++) {
                if (RandomFloat(0.0f, 1.0f) < MUTATION_RATE) {
                    weightsHO[h][o] += RandomFloat(-1.0f, 1.0f) * strength;
                    if (weightsHO[h][o] >  WEIGHT_CLAMP) weightsHO[h][o] =  WEIGHT_CLAMP;
                    if (weightsHO[h][o] < -WEIGHT_CLAMP) weightsHO[h][o] = -WEIGHT_CLAMP;
                }
            }
            if (RandomFloat(0.0f, 1.0f) < MUTATION_RATE)
                biasH[h] += RandomFloat(-0.5f, 0.5f) * strength;
        }

        for (int o = 0; o < OUTPUT_COUNT; o++) {
            if (RandomFloat(0.0f, 1.0f) < MUTATION_RATE)
                biasO[o] += RandomFloat(-0.5f, 0.5f) * strength;
        }
    }

    // Inputs [13]:
    //  0  dist/30               normalised distance to player
    //  1  sin(angle)            y-component to player
    //  2  cos(angle)            x-component to player
    //  3  player.angle/PI       player facing direction
    //  4  health/maxHP          self HP ratio
    //  5  nearbyCount/10        nearby allies
    //  6  isMoving              player movement flag
    //  7  currentWeapon/3       weapon type
    //  8  survivalTime/60       age of this brain
    //  9  nearestBulletDist/20  bullet danger (0=close,1=far/none)
    // 10  nearestBulletAngle/PI incoming bullet direction
    // 11  wallAhead (-1/+1)     wall obstacle directly ahead
    // 12  generation/50         generational pressure
    //
    // Outputs [5]:
    //  0  moveBias    approach (+) / retreat (-)
    //  1  strafe      lateral evasion magnitude
    //  2  aggression  attack-range modifier
    //  3  dodgeBoost  burst speed perpendicular to bullet
    //  4  coverSeek   bias toward wall-adjacent cover
    void Evaluate(const float inputs[INPUT_COUNT], float outputs[OUTPUT_COUNT]) const {
        float hidden[HIDDEN_COUNT];

        for (int h = 0; h < HIDDEN_COUNT; h++) {
            float sum = biasH[h];
            for (int i = 0; i < INPUT_COUNT; i++)
                sum += inputs[i] * weightsIH[i][h];
            hidden[h] = tanhf(sum);
        }

        for (int o = 0; o < OUTPUT_COUNT; o++) {
            float sum = biasO[o];
            for (int h = 0; h < HIDDEN_COUNT; h++)
                sum += hidden[h] * weightsHO[h][o];
            outputs[o] = tanhf(sum);
        }
    }

    // Fitness:
    //   Reward landing damage heavily.
    //   Reward survival only when the enemy actually fought.
    //   Penalise cowardly survival (never attacked).
    //   Reward successful bullet evasions.
    void UpdateFitness() {
        if (damageDealt > 0.0f) {
            // Brave enemy that actually engaged the player
            fitness = damageDealt * 15.0f + survivalTime + (float)bulletsEvaded * 5.0f;
        } else {
            // Enemy that didn't deal damage — may have learned useful evasion
            // Still heavily penalised vs attackers, but don't trash evasion skills
            fitness = survivalTime * 0.05f + (float)bulletsEvaded * 2.0f;
        }
    }
};

// ---------------------------------------------------------------------------
// Global evolutionary state
// ---------------------------------------------------------------------------
static NeuralNet populationPool[POOL_SIZE];
static int       brainsInPool      = 0;
static float     globalBestFitness = 0.0f;
static int       generation        = 1;
static bool      brainInitialized  = false;

inline void InitGlobalBrain() {
    if (!brainInitialized) {
        populationPool[0].Randomize();
        brainsInPool = 1;
        brainInitialized = true;
    }
}

inline void UpdateGlobalBest(NeuralNet& brain) {
    brain.UpdateFitness();
    
    if (brainsInPool < POOL_SIZE) {
        populationPool[brainsInPool].CopyFrom(brain);
        populationPool[brainsInPool].fitness = brain.fitness;
        brainsInPool++;
        std::sort(populationPool, populationPool + brainsInPool, [](const NeuralNet& a, const NeuralNet& b) {
            return a.fitness > b.fitness;
        });
    } else if (brain.fitness > populationPool[POOL_SIZE - 1].fitness) {
        populationPool[POOL_SIZE - 1].CopyFrom(brain);
        populationPool[POOL_SIZE - 1].fitness = brain.fitness;
        std::sort(populationPool, populationPool + POOL_SIZE, [](const NeuralNet& a, const NeuralNet& b) {
            return a.fitness > b.fitness;
        });
    }
    
    if (brainsInPool > 0) {
        globalBestFitness = populationPool[0].fitness;
    }
}

// Inherit from the top pool. Mutation scale is higher in early
// generations to promote exploration, tapering as the brain matures.
inline void InheritBrain(NeuralNet& brain, int gen = 1) {
    InitGlobalBrain();
    int parentIdx = RandomInt(0, brainsInPool - 1);
    brain.CopyFrom(populationPool[parentIdx]);
    
    float mutScale = (gen < 10) ? 1.5f : (gen < 30 ? 1.1f : 1.0f);
    brain.Mutate(mutScale);
}

inline void NextGeneration() { generation++; }
inline int   GetGeneration()  { return generation; }
inline float GetBestFitness() { return globalBestFitness; }

// ---------------------------------------------------------------------------
// Persistence  (binary, little-endian)
// Layout: [magic 4B][version i32][generation i32][poolSize i32]
//         ...pool elements
// ---------------------------------------------------------------------------
static const int NN_FILE_MAGIC   = 0x49414E4E; // "NNAI"
static const int NN_FILE_VERSION = 3;           // Bumped for expanded topology and population

inline bool SaveBrain(const wchar_t* path) {
    if (!path || !brainInitialized || brainsInPool == 0) return false;

    FILE* f = nullptr;
    if (_wfopen_s(&f, path, L"wb") != 0 || !f) return false;

    bool ok = true;
    ok = ok && (fwrite(&NN_FILE_MAGIC,   sizeof(int), 1, f) == 1);
    ok = ok && (fwrite(&NN_FILE_VERSION, sizeof(int), 1, f) == 1);
    ok = ok && (fwrite(&generation,      sizeof(int), 1, f) == 1);
    ok = ok && (fwrite(&brainsInPool,    sizeof(int), 1, f) == 1);
    
    for (int i = 0; i < brainsInPool; ++i) {
        ok = ok && (fwrite(&populationPool[i].fitness, sizeof(float), 1, f) == 1);
        ok = ok && (fwrite(populationPool[i].weightsIH, sizeof(populationPool[i].weightsIH), 1, f) == 1);
        ok = ok && (fwrite(populationPool[i].weightsHO, sizeof(populationPool[i].weightsHO), 1, f) == 1);
        ok = ok && (fwrite(populationPool[i].biasH,     sizeof(populationPool[i].biasH),     1, f) == 1);
        ok = ok && (fwrite(populationPool[i].biasO,     sizeof(populationPool[i].biasO),     1, f) == 1);
    }

    fclose(f);
    return ok;
}

inline bool LoadBrain(const wchar_t* path) {
    if (!path) return false;

    FILE* f = nullptr;
    if (_wfopen_s(&f, path, L"rb") != 0 || !f) return false;

    try {
        int magic = 0, version = 0;
        if (fread(&magic, sizeof(int), 1, f) != 1 || fread(&version, sizeof(int), 1, f) != 1) {
            throw std::runtime_error("Header read failed");
        }

        if (magic != NN_FILE_MAGIC || version != NN_FILE_VERSION) {
            throw std::runtime_error("Version mismatch");
        }

        if (fread(&generation, sizeof(int), 1, f) != 1 || generation < 1) {
            throw std::runtime_error("Invalid generation");
        }

        if (fread(&brainsInPool, sizeof(int), 1, f) != 1 || brainsInPool < 1 || brainsInPool > POOL_SIZE) {
            throw std::runtime_error("Invalid pool size");
        }

        for (int i = 0; i < brainsInPool; ++i) {
            if (fread(&populationPool[i].fitness, sizeof(float), 1, f) != 1 || std::isnan(populationPool[i].fitness)) {
                throw std::runtime_error("Invalid fitness");
            }
            if (fread(populationPool[i].weightsIH, sizeof(populationPool[i].weightsIH), 1, f) != 1) throw std::runtime_error("Read error weightsIH");
            if (fread(populationPool[i].weightsHO, sizeof(populationPool[i].weightsHO), 1, f) != 1) throw std::runtime_error("Read error weightsHO");
            if (fread(populationPool[i].biasH, sizeof(populationPool[i].biasH), 1, f) != 1) throw std::runtime_error("Read error biasH");
            if (fread(populationPool[i].biasO, sizeof(populationPool[i].biasO), 1, f) != 1) throw std::runtime_error("Read error biasO");

            populationPool[i].survivalTime  = 0.0f;
            populationPool[i].damageDealt   = 0.0f;
            populationPool[i].bulletsEvaded = 0;
        }

        if (brainsInPool > 0) {
            globalBestFitness = populationPool[0].fitness;
        }
        
        brainInitialized = true;
        fclose(f);
        return true;
    } catch (...) {
        fclose(f);
        return false; // Version mismatch or corruption — start fresh
    }
}

} // namespace NeuralAI

#endif
