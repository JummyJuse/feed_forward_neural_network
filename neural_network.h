#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <sstream>

using namespace std;

/*Heres this random function (I changed it from a normal distribution
to add more varience to the weights rather than have them bunch up to zero*/
double random(double min, double max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(min, max);
    return dist(gen);
}

template<typename T>
void printVector(const T& elem, int indentationLevel = 0) {
    for (int i = 0; i < indentationLevel; ++i) cout << "  ";
    cout << elem << endl;
}

template<typename T>
void printVector(const vector<T>& vec, int indentationLevel = 0) {
    for (int i = 0; i < indentationLevel; ++i) cout << "  ";
    cout << "[ " << endl;

    for (const auto& elem : vec) {
        printVector(elem, indentationLevel + 1);
    }

    for (int i = 0; i < indentationLevel; ++i) cout << "  ";
    cout << "]" << endl;
}

/*The big daddy structure*/
struct nn {
    /*Tells the network to impliment gradient clipping*/
    bool gClipping = false;
    /*Tells the network to impliment linear activation function*/
    bool linearActivation = false;
    /*The learning rate of the system tells it how much to change the parameters during training.
    Since the training step is linear, and the actual gradient decent is non-linear, its best to keep this value small.
    a common range is from 0.006-0.120
    */
    float learningRate = 0.01;
    /*The neuron's values. Not so uncommon as the water in a pipe system, or the electrons in a computer*/
    vector<vector<float>> neuronValues;
    vector<vector<vector<float>>> weights;
    vector<vector<float>> biases;

    /*Just in case the neural network tries to pop out HUGE weights and biases.
    It minimizes the gradients in the back propigation cus if a desired output is bigger than 1,
    when calculating the error, you'd be capping it out at 1 anyways so the difference would make the gradient huge
    dE/d(W or B) = 1/n (not really but it will scale like that) */
    float clipGradient(float grad, float minVal, float maxVal) {
        if (gClipping) {
            if (grad < minVal) return minVal;
            if (grad > maxVal) return maxVal;
        }
        return grad;
    }

    /*If you're this deep, you prolly know what this does but heres a quick overview.
    lim(x-> -inf) s(x) = -1,
    lim(x-> inf) s(x) = 1                 */
    float sigmoid(float x) {
        if (linearActivation) {
            return x; //just in case u want big numbers in the neural net and the output (keep giving it "binary classification datasets")
        }
        else {
            return 1 / (1 + exp(-x));
        }
    }
    /*The derivative of sigmoid. Used for chain rule when calculating partial derivative of the */
    float sigmoidDerivative(float x) {
        float sig = sigmoid(x);
        if (linearActivation) {
            return 1; //for a linear activation function, linear = derivitive is 1.
        }
        else {
            return sig * (1 - sig);
        }
    }

    /*The "scalable" aspect of this application.
    nn.configlayer(0, 13); //layer 1 (input layer with 13 perceptrons)
    nn.configlayer(1, 61); //layer 2 (61 neurons)
    nn.configlayer(2, 38); //layer 3 (38 neurons)
    nn.configlayer(3, 6); //layer 4 (output layer with 6 perceptrons)
    a network can have as little as 2 layers*/
    void configlayer(const int layernum, const int inp) {
        vector<float> preassignedLayer;
        for (int i = 0; i < inp; i++) {
            preassignedLayer.push_back(0.0);
        }
        while (neuronValues.size() < layernum + 1) {
            neuronValues.push_back({ 0 });
        }
        neuronValues[layernum] = preassignedLayer;
    }

    /*Sets the input perceptrons/neurons idk into the vector inputted.
    Make sure vector is the same size as the nn.configlayer(0, size) */
    void setinput(vector<float> inputs) {
        if (inputs.size() == neuronValues[0].size()) {
            neuronValues[0] = inputs;
        }
        else {
            cerr << "\nERROR: INPUT SET IS DIFFERENT THAN NETWORKS INPUT CAPACITY\n";
        }
    }

    /*Crucial when setting up the neural network. Sets up each weight and bias a random value between -1 and 1.
    You can theoretically also "reset" a network by calling it too. But Idk why i didnt make this a part
    of the configlayer function but que sera sera*/
    void initWBs() {
        vector<vector<vector<float>>> newWeights;
        vector<vector<float>> newBiases;

        for (int i = 0; i < neuronValues.size() - 1; i++) {
            newWeights.push_back({});
            for (int j = 0; j < neuronValues[i].size(); j++) {
                newWeights[i].push_back({});
                for (int k = 0; k < neuronValues[i + 1].size(); k++) {
                    newWeights[i][j].push_back(random(-1, 1));
                }
            }
        }

        for (int i = 0; i < neuronValues.size(); i++) {
            newBiases.push_back({});
            for (int j = 0; j < neuronValues[i].size(); j++) {
                newBiases[i].push_back(random(-1, 1));
            }
        }
        weights = newWeights;
        biases = newBiases;
    }
    /*The running function of the neural network. Typically you'd call this after you set the inputs.
    After doing this, the outer most layer of neuronValues is your output.
    It works by forward propagating the next layer by adding the previous layer times the weights plus the biases (plugged into an activation function)*/
    void epoch() {
        for (int i = 1; i < neuronValues.size(); i++) {
            for (int j = 0; j < neuronValues[i].size(); j++) {
                float totalNeuronInput = biases[i][j];
                for (int k = 0; k < neuronValues[i - 1].size(); k++) {
                    totalNeuronInput = totalNeuronInput + (neuronValues[i - 1][k] * weights[i - 1][k][j]);
                }
                neuronValues[i][j] = sigmoid(totalNeuronInput);
            }
        }
    }
    /*The infamous training method. Responsible for updating the weights and biases to edge the network towards matching
    the target output over time. Takes in an a target output and compares it to the output created after the epoch() was called.
    */
    float train(vector<float> targetOutputs) {
        vector<vector<vector<float>>> newWeights = weights;
        vector<vector<float>> newBiases = biases;
        /*How much the neurons given their activation have an effect on the error of the network
        Basically the instantanious rate of change of the error at a certain activation*/
        vector<vector<float>> delta(neuronValues.size());

        int outputLayer = neuronValues.size() - 1; //creating an index value for the output layer

        delta[outputLayer].resize(neuronValues[outputLayer].size());

        for (int j = 0; j < neuronValues[outputLayer].size(); j++) {
            float output = neuronValues[outputLayer][j];
            float error = output - targetOutputs[j];
            delta[outputLayer][j] = error * sigmoidDerivative(output);
        }

        //Using a summation to add each weight's individual impact on the system to their respective neuron so I dont have to derivate 
        //more complex stuff the further i get into the network.
        for (int i = outputLayer - 1; i >= 0; i--) {
            delta[i].resize(neuronValues[i].size());
            for (int j = 0; j < neuronValues[i].size(); j++) {
                float errorSum = 0;
                for (int k = 0; k < neuronValues[i + 1].size(); k++) {
                    errorSum += delta[i + 1][k] * weights[i][j][k]; 
                }
                delta[i][j] = errorSum * sigmoidDerivative(neuronValues[i][j]);
            }
        }

        //Update the weights by subtracting their respecive gradient DESCENT times the learning rate 
        for (int i = 0; i < neuronValues.size() - 1; i++) {
            for (int j = 0; j < neuronValues[i].size(); j++) {
                for (int k = 0; k < neuronValues[i + 1].size(); k++) {
                    float gradientDescentW = clipGradient(delta[i + 1][k] * neuronValues[i][j], -1, 1);
                    newWeights[i][j][k] -= learningRate * gradientDescentW;
                }
            }
        }

        //Same thing for the biases except without factoring the neurons values into the gradient
        for (int i = 0; i < neuronValues.size(); i++) {
            for (int j = 0; j < neuronValues[i].size(); j++) {
                float gradientDescentB = clipGradient(delta[i][j], -1, 1);
                newBiases[i][j] -= learningRate * gradientDescentB;
            }
        }
        
        //Replacing the weights and biases with the *new* weights and biases
        weights = newWeights;
        biases = newBiases;
        return 1;
    }
    vector<float> run(vector<float>& givenInput) {
        setinput(givenInput);
        epoch();
        return neuronValues[neuronValues.size() - 1];
    }
    vector<float> learn(vector<float> givenInput, vector<float> givenExpected) {
        vector<float> outs = run(givenInput);
        train(givenExpected);
        return outs;
    }
};

/*A flashcard type of implimentation system that takes a neural network and trains it.*/
struct trainingSet {
    vector<vector<float>> trainingInputs;
    vector<vector<float>> trainingOutputs;

    /*Adds a flashcard to the dataset for any given network to study*/
    void flashcard(vector<float> input, vector<float> output) {
        trainingInputs.push_back(input);
        trainingOutputs.push_back(output);
    }
    /*Takes in a neuralnet as an arguement and makes it study the flashcards a given amount of times*/
    float study(nn& network, int iteration) {
        if (network.biases.size() != network.neuronValues.size()) network.initWBs();
        int casenum = 0;
        float score = 0;
        for (int i = 0; i < iteration; i++) {  //iterates through i and assignes the network to train from a random flashcard each time
            vector<float> inputs, expected;
            casenum = floor(random(0, trainingInputs.size()));
            inputs = trainingInputs[casenum];
            expected = trainingOutputs[casenum];
            vector<float> outputs = network.learn(inputs, expected);
            float rawScore = 0;
            for (int i = 0; i < expected.size(); i++) {
                rawScore += ((outputs[i] - expected[i]) * (outputs[i] - expected[i]));
            }
            score = score + rawScore / inputs.size();
            if (iteration > 100) {
                if (((i + 1) % (iteration / 100)) == 0) {
                    //have it do something 100 times as a loading screen protocal
                }
            }
        }
        return 100 * (1 - (score / iteration));
    }
};
