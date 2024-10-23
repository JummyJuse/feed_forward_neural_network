//#include <iostream>
//#include <vector>
//#include <cmath>
//#include <random>
//#include <iomanip>
//#include <sstream>
//using namespace std;
//
//double random(double min, double max) {
//	random_device rd;
//	mt19937 gen(rd());
//	uniform_real_distribution<double> dist(min, max);
//	return dist(gen);
//}
//
//template<typename T>
//void printVector(const T& elem, int indentationLevel = 0) {
//	for (int i = 0; i < indentationLevel; ++i) cout << "  ";
//	cout << elem << endl;
//}
//template<typename T>
//void printVector(const vector<T>& vec, int indentationLevel = 0) {
//	for (int i = 0; i < indentationLevel; ++i) cout << "  ";
//	cout << "[ " << endl;
//
//	for (const auto& elem : vec) {
//		printVector(elem, indentationLevel + 1);
//	}
//
//	for (int i = 0; i < indentationLevel; ++i) cout << "  ";
//	cout << "]" << endl;
//}
//
//struct nn {
//	bool gClipping = false;
//	bool linAc = false;
//	float clipGradient(float grad, float minVal, float maxVal) {
//		if (gClipping) {
//			if (grad < minVal) return minVal;
//			if (grad > maxVal) return maxVal;
//		}
//		return grad;
//	}
//	float learningRate = 0.25;
//	vector<vector<float>> neuronValues;
//	vector<vector<vector<float>>> weights;
//	vector<vector<float>> biases;
//
//	float sigmoid(float x) {
//		if (linAc) {
//			return x;
//		}
//		else {
//			return 1 / (1 + exp(-x));
//		}
//	}
//	float sigmoidDerivative(float x) {
//		float sig = sigmoid(x);
//		if (linAc) {
//			return 1;
//		}
//		else {
//			return sig * (1 - sig);
//		}
//	}
//
//	void configlayer(const int layernum, const int inp) {
//		vector<float> preassignedLayer;
//
//		for (int i = 0; i < inp; i++) {
//			preassignedLayer.push_back(0.0);
//		}
//		while (neuronValues.size() < layernum + 1) {
//				neuronValues.push_back({ 0 });
//
//		}
//		neuronValues[layernum] = preassignedLayer;
//
//	}
//
//	void setinput(vector<float> inputs) {
//		if (inputs.size() == neuronValues[0].size()) {
//			neuronValues[0] = inputs;
//		}
//		else {
//			cerr << "\nERROR: INPUT SET IS DIFFERENT THAN NETWORKS INPUT CAPACATY\n";
//		}
//	}
//	void initWBs() {
//
//		vector<vector<vector<float>>> newWeights;
//		vector<vector<float>> newBiases;
//
//		for (int i = 0; i < neuronValues.size() - 1; i++) {
//			newWeights.push_back({});
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				newWeights[i].push_back({});
//				for (int k = 0; k < neuronValues[i + 1].size(); k++) {
//					newWeights[i][j].push_back(random(-1, 1));
//				}
//			}
//		}
//
//		for (int i = 0; i < neuronValues.size(); i++) {
//			newBiases.push_back({});
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				newBiases[i].push_back(random(-1, 1));
//			}
//		}
//		weights = newWeights;
//		biases = newBiases;
//
//	};
//
//	void epoch() {
//		for (int i = 1; i < neuronValues.size(); i++) {
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				float totalNeuronInput = biases[i][j];
//				for (int k = 0; k < neuronValues[i - 1].size(); k++) {
//					totalNeuronInput = totalNeuronInput + (neuronValues[i - 1][k] * weights[i - 1][k][j]);
//				}
//				neuronValues[i][j] = sigmoid(totalNeuronInput);
//			}
//		}
//	}
//
//	float train(vector<float> targetOutputs) {
//		vector<vector<vector<float>>> newWeights = weights;
//		vector<vector<float>> newBiases = biases;
//		vector<vector<float>> delta(neuronValues.size()); 
//
//		int outputLayer = neuronValues.size() - 1;
//		delta[outputLayer].resize(neuronValues[outputLayer].size());
//
//		for (int j = 0; j < neuronValues[outputLayer].size(); j++) {
//			float output = neuronValues[outputLayer][j];
//			float error = output - targetOutputs[j]; 
//			delta[outputLayer][j] = error * sigmoidDerivative(output);
//		}
//
//		for (int i = outputLayer - 1; i >= 0; i--) {
//			delta[i].resize(neuronValues[i].size());
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				float errorSum = 0;
//				for (int k = 0; k < neuronValues[i + 1].size(); k++) {
//					errorSum += delta[i + 1][k] * weights[i][j][k];
//				}
//				delta[i][j] = errorSum * sigmoidDerivative(neuronValues[i][j]);
//			}
//		}
//		for (int i = 0; i < neuronValues.size() - 1; i++) {
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				for (int k = 0; k < neuronValues[i + 1].size(); k++) {
//					float gradientDecentW = clipGradient(delta[i + 1][k] * neuronValues[i][j], -1, 1);
//					newWeights[i][j][k] -= learningRate * gradientDecentW;
//				}
//			}
//		}
//
//		for (int i = 0; i < neuronValues.size(); i++) {
//			for (int j = 0; j < neuronValues[i].size(); j++) {
//				float gradientDecentB = clipGradient(delta[i][j], -1, 1);
//				newBiases[i][j] -= learningRate * gradientDecentB;
//			}
//		}
//
//		weights = newWeights;
//		biases = newBiases;
//		int correctPredictions = 0;
//		int totalPredictions = targetOutputs.size();
//
//		for (int j = 0; j < neuronValues[outputLayer].size(); j++) {
//			float output = neuronValues[outputLayer][j];
//			float predicted = output >= 0.5 ? 1.0f : 0.0f;
//			if (predicted == targetOutputs[j]) {
//				correctPredictions++;
//			}
//		}
//		float accuracy = static_cast<float>(correctPredictions) / totalPredictions;
//
//		return accuracy;
//	}
//
//};
//struct trainingSet {
//	vector<vector<float>> trainingInputs;
//	vector<vector<float>> trainingOutputs;
//
//	void initCase(vector<float> input, vector<float> output) {
//		trainingInputs.push_back(input);
//		trainingOutputs.push_back(output);
//	}
//	float itorateTraining(nn &network, int itoration) {
//		int casenum = 0;
//		float score = 0;
//		for (int i = 0; i < itoration; i++) {
//			vector<float> inputs, expected;
//			casenum = floor(random(0, trainingInputs.size()));
//			inputs = trainingInputs[casenum];
//			expected = trainingOutputs[casenum];
//			network.setinput(inputs);
//			network.epoch();
//			network.train(expected);
//			float rawScore = 0;
//			for (int i = 0; i < expected.size(); i++) {
//				rawScore += ((network.neuronValues[network.neuronValues.size() - 1][i] - expected[i]) * (network.neuronValues[network.neuronValues.size() - 1][i] - expected[i]));
//			}
//			score = score + rawScore / inputs.size();
//			if ((i % (itoration/100)) == 0) {
//				system("cls");
//				cout << "iteration: " << i << " / " << itoration << " Fitness: " << 100 * (1 - score/i) << endl;
//				printVector(network.weights);
//			}
//		}
//		system("cls");
//		cout << "iteration: " << itoration << " Fitness: " << 100 * (1 - score / itoration) << endl;
//		printVector(network.weights);
//		return 100 * (1 - (score/itoration));
//		
//	}
//};
//
//int main() {
//	nn nn1;
//	nn1.configlayer(0, 4);
//	nn1.configlayer(1, 9);
//	nn1.configlayer(2, 4);
//	for (int num = 0; num < nn1.neuronValues.size(); num++) {
//		cout << "layer " << num + 1 << " size: " << nn1.neuronValues[num].size() << " neurons" << endl;
//	}
//	nn1.initWBs();
//	nn1.learningRate = 0.001;
//	nn1.linAc = true;
//	nn1.gClipping = false;
//	trainingSet adderLogic;	
//	adderLogic.initCase({ 0,1,1,0 }, { 0,1,0,1 });
//	adderLogic.initCase({ 1,0,0,0 }, { 1,1,0,1 });
//	adderLogic.initCase({ 0,1,0,0 }, { 0,1,1,1 });
//	adderLogic.initCase({ 0,0,1,0 }, { 0,1,0,1 });
//	adderLogic.initCase({ 1,0,1,0 }, { 0,1,0,0 });
//	adderLogic.initCase({ 1,0,1,1 }, { 0,0,1,0 });
//	adderLogic.initCase({ 1,1,1,0 }, { 0,0,0,1 });
//
//
//
//	float score2 = adderLogic.itorateTraining(nn1, 100000);
//
//	cout << endl << fixed << setprecision(6) << "Network fitness: " << score2 << " % ";
//	while (true) {
//		vector<float> inputter;
//		for (int i = 0; i < nn1.neuronValues[0].size(); i++) {
//			float response;
//			cout << "\nNeuron " << i + 1 << ": ";
//			cin >> response;
//			inputter.push_back(response);
//		}
//		nn1.setinput(inputter);
//		nn1.epoch();
//
//		cout << "\nPrediction: [ ";
//		for (int i = 0; i < nn1.neuronValues[nn1.neuronValues.size() - 1].size(); i++) {
//			string iscomma = "";
//			if (i == nn1.neuronValues[nn1.neuronValues.size() - 1].size() - 1) {
//				iscomma = " ";
//			}
//			else {
//				iscomma = ", ";
//			}
//			cout << setprecision(3) << abs(round(nn1.neuronValues[nn1.neuronValues.size() - 1][i])) << iscomma;
//		}
//		cout << "]\n";
//	}
//
//}
