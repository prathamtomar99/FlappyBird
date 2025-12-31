#pragma once
#include <vector>
#include <string>

class NeuralNetwork {
public:
    NeuralNetwork();

    float forward(const std::vector<float>& input);
    void mutate(float rate, float strength);

    void save(const std::string& file);
    void load(const std::string& file);

private:
    std::vector<std::vector<float>> w1; // 5 x 8
    std::vector<float> b1;              // 8
    std::vector<float> w2;              // 8
    float b2;

    float tanh(float x);
    float sigmoid(float x);
};
