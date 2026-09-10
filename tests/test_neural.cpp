// tests/test_neural.cpp
// Unit tests for NeuralAI::NeuralNet in neural.hpp
// Run via: make test

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>
#include <cwchar>
#include <windows.h>

#include "neural.hpp"

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

// -----------------------------------------------------------------
// Test 1: Randomize produces weights in [-1, 1]
// -----------------------------------------------------------------
void test_randomize_range() {
    printf("Test 1: Randomize weight range\n");
    NeuralAI::NeuralNet net;
    net.Randomize();

    bool ok = true;
    for (int i = 0; i < NeuralAI::INPUT_COUNT && ok; i++)
        for (int h = 0; h < NeuralAI::HIDDEN_COUNT && ok; h++)
            if (net.weightsIH[i][h] < -1.0f || net.weightsIH[i][h] > 1.0f) ok = false;
    for (int h = 0; h < NeuralAI::HIDDEN_COUNT && ok; h++)
        for (int o = 0; o < NeuralAI::OUTPUT_COUNT && ok; o++)
            if (net.weightsHO[h][o] < -1.0f || net.weightsHO[h][o] > 1.0f) ok = false;

    CHECK(ok,              "weightsIH and weightsHO in [-1,1] after Randomize");
    CHECK(net.fitness       == 0.0f, "fitness zero after Randomize");
    CHECK(net.survivalTime  == 0.0f, "survivalTime zero after Randomize");
    CHECK(net.damageDealt   == 0.0f, "damageDealt zero after Randomize");
    CHECK(net.bulletsEvaded == 0,    "bulletsEvaded zero after Randomize");
}

// -----------------------------------------------------------------
// Test 2: Evaluate with zero inputs produces bounded outputs
// -----------------------------------------------------------------
void test_evaluate_bounded() {
    printf("Test 2: Evaluate output bounds\n");
    NeuralAI::NeuralNet net;
    net.Randomize();

    float inputs[NeuralAI::INPUT_COUNT] = {};   // all zeros
    float outputs[NeuralAI::OUTPUT_COUNT] = {};
    net.Evaluate(inputs, outputs);

    bool bounded = true, finite_ok = true;
    for (int o = 0; o < NeuralAI::OUTPUT_COUNT; o++) {
        if (outputs[o] < -1.0f || outputs[o] > 1.0f) bounded = false;
        if (!std::isfinite(outputs[o]))               finite_ok = false;
    }
    CHECK(bounded,   "all outputs in [-1,1]");
    CHECK(finite_ok, "no NaN or Inf in outputs");
}

// -----------------------------------------------------------------
// Test 3: Mutate clamps weights to [-WEIGHT_CLAMP, WEIGHT_CLAMP]
// -----------------------------------------------------------------
void test_mutate_clamp() {
    printf("Test 3: Mutate weight clamping\n");
    NeuralAI::NeuralNet net;
    net.Randomize();

    // Mutate many times to stress clamping
    for (int i = 0; i < 200; i++) net.Mutate(5.0f); // High scale to push values

    bool clamped = true;
    for (int i = 0; i < NeuralAI::INPUT_COUNT && clamped; i++)
        for (int h = 0; h < NeuralAI::HIDDEN_COUNT && clamped; h++)
            if (net.weightsIH[i][h] < -NeuralAI::WEIGHT_CLAMP ||
                net.weightsIH[i][h] >  NeuralAI::WEIGHT_CLAMP) clamped = false;
    for (int h = 0; h < NeuralAI::HIDDEN_COUNT && clamped; h++)
        for (int o = 0; o < NeuralAI::OUTPUT_COUNT && clamped; o++)
            if (net.weightsHO[h][o] < -NeuralAI::WEIGHT_CLAMP ||
                net.weightsHO[h][o] >  NeuralAI::WEIGHT_CLAMP) clamped = false;

    CHECK(clamped, "weights clamped to [-WEIGHT_CLAMP, WEIGHT_CLAMP] after heavy mutation");
}

// -----------------------------------------------------------------
// Test 4: UpdateFitness orders two brains correctly
// -----------------------------------------------------------------
void test_fitness_ordering() {
    printf("Test 4: Fitness ordering\n");
    NeuralAI::NeuralNet aggressive, coward;

    aggressive.damageDealt   = 50.0f;
    aggressive.survivalTime  = 10.0f;
    aggressive.bulletsEvaded = 3;
    aggressive.fitness       = 0;
    aggressive.UpdateFitness();

    coward.damageDealt   = 0.0f;
    coward.survivalTime  = 60.0f;  // Survived longer but never attacked
    coward.bulletsEvaded = 0;
    coward.fitness       = 0;
    coward.UpdateFitness();

    CHECK(aggressive.fitness > coward.fitness,
          "aggressive brain scores higher than coward brain");
    CHECK(aggressive.fitness > 0.0f, "aggressive fitness is positive");
    CHECK(coward.fitness     > 0.0f, "coward fitness is positive (survival bonus)");
}

// -----------------------------------------------------------------
// Test 5: CopyFrom produces identical fitness after UpdateFitness
// -----------------------------------------------------------------
void test_copy_fitness() {
    printf("Test 5: CopyFrom + UpdateFitness equivalence\n");
    NeuralAI::NeuralNet src, dst;
    src.Randomize();
    src.damageDealt   = 20.0f;
    src.survivalTime  = 15.0f;
    src.bulletsEvaded = 2;
    src.UpdateFitness();

    dst.CopyFrom(src);
    // After CopyFrom, metrics are reset; restore and recompute
    dst.damageDealt   = 20.0f;
    dst.survivalTime  = 15.0f;
    dst.bulletsEvaded = 2;
    dst.UpdateFitness();

    CHECK(fabsf(dst.fitness - src.fitness) < 1e-5f,
          "copied brain has same fitness as original");

    // Weights should be bit-identical
    bool same = (memcmp(dst.weightsIH, src.weightsIH, sizeof(src.weightsIH)) == 0 &&
                 memcmp(dst.weightsHO, src.weightsHO, sizeof(src.weightsHO)) == 0 &&
                 memcmp(dst.biasH,     src.biasH,     sizeof(src.biasH))     == 0 &&
                 memcmp(dst.biasO,     src.biasO,     sizeof(src.biasO))     == 0);
    CHECK(same, "copied weights are bit-identical to source");
}

// -----------------------------------------------------------------
// Test 6: SaveBrain / LoadBrain round-trip
// -----------------------------------------------------------------
void test_save_load_roundtrip() {
    printf("Test 6: SaveBrain / LoadBrain round-trip\n");

    // Reset global state
    NeuralAI::NeuralNet origBrain;
    origBrain.Randomize();
    origBrain.damageDealt   = 30.0f;
    origBrain.survivalTime  = 20.0f;
    origBrain.bulletsEvaded = 5;
    origBrain.UpdateFitness();

    NeuralAI::InitGlobalBrain();
    NeuralAI::UpdateGlobalBest(origBrain);
    int origGen = NeuralAI::GetGeneration();

    // Build a temp file path guaranteed to be writable
    wchar_t tmpDir[MAX_PATH];
    GetTempPathW(MAX_PATH, tmpDir);
    wchar_t tmp[MAX_PATH];
    swprintf(tmp, MAX_PATH, L"%lstest_brain_tmp.dat", tmpDir);

    bool saved = NeuralAI::SaveBrain(tmp);
    CHECK(saved, "SaveBrain returns true");

    // Corrupt global state, then reload
    NeuralAI::NeuralNet blank;
    blank.Randomize();
    NeuralAI::UpdateGlobalBest(blank);

    bool loaded = NeuralAI::LoadBrain(tmp);
    CHECK(loaded, "LoadBrain returns true");
    CHECK(NeuralAI::GetGeneration() == origGen, "generation restored after LoadBrain");
    CHECK(fabsf(NeuralAI::GetBestFitness() - origBrain.fitness) < 1e-4f,
          "bestFitness restored after LoadBrain");

    _wremove(tmp);
}

// -----------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------
int main() {
    printf("=== NeuralAI Unit Tests ===\n\n");

    test_randomize_range();
    test_evaluate_bounded();
    test_mutate_clamp();
    test_fitness_ordering();
    test_copy_fitness();
    test_save_load_roundtrip();

    printf("\n%d / %d tests passed.\n", g_passed, g_tests);
    return (g_passed == g_tests) ? 0 : 1;
}
