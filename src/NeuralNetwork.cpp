#include "NeuralNetwork.h"
#include <random>
#include <fstream>
#include <cmath>
#include <iostream>

static std::mt19937 rng(std::random_device{}());
static std::uniform_real_distribution<float> dist(-1.f, 1.f);

NeuralNetwork::NeuralNetwork() {
    w1.resize(5, std::vector<float>(8));
    b1.resize(8);
    w2.resize(8);

    for (auto& row : w1)
        for (float& v : row) v = dist(rng);

    for (float& v : b1) v = dist(rng);
    for (float& v : w2) v = dist(rng);
    b2 = dist(rng);

}

float NeuralNetwork::tanh(float x) {
    return std::tanh(x);
}

float NeuralNetwork::sigmoid(float x) {
    return 1.f / (1.f + std::exp(-x));
}

float NeuralNetwork::forward(const std::vector<float>& in) {
    std::vector<float> hidden(8, 0.f);

    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 5; i++)
            hidden[j] += in[i] * w1[i][j];
        hidden[j] += b1[j];
        hidden[j] = tanh(hidden[j]);
    }

    float out = 0.f;
    for (int j = 0; j < 8; j++)
        out += hidden[j] * w2[j];

    out += b2;
    return tanh(out);
}

void NeuralNetwork::mutate(float rate, float strength) {
    std::normal_distribution<float> noise(0.f, strength);

    auto mutateVal = [&](float& v) {
        if ((rand() / (float)RAND_MAX) < rate)
            v += noise(rng);
    };

    for (auto& row : w1)
        for (float& v : row) mutateVal(v);

    for (float& v : b1) mutateVal(v);
    for (float& v : w2) mutateVal(v);

    mutateVal(b2);
}

void NeuralNetwork::save(const std::string& file) {
    std::ofstream out(file);
    for (auto& row : w1)
        for (float v : row) out << v << " ";
    for (float v : b1) out << v << " ";
    for (float v : w2) out << v << " ";
    out << b2;
}

void NeuralNetwork::load(const std::string& file) {
    std::ifstream in(file);
    for (auto& row : w1)
        for (float& v : row) in >> v;
    for (float& v : b1) in >> v;
    for (float& v : w2) in >> v;
    in >> b2;
}
