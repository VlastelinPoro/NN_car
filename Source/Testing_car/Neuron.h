#pragma once

#include "CoreMinimal.h"
#include <vector>

using namespace std;
/**
 * 
 */
class Neuron
{
public:
	int numInputs;
	double bias;
	double output;
	double errorGradient;
	vector<double> weights = {};
	vector<double> inputs = {};
	Neuron(int _numInputs);
	~Neuron();
};
