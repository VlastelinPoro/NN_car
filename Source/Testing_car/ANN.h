// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NNLayer.h"

/**
 * 
 */
class ANN
{
public:
	ANN();
	ANN(int _numInputs, int _numOutputs, int _numHidden,int _numNeuronPerHidden, double P, double L);
	~ANN();
	vector<FString> Split(FString line, FString delit);
	vector<double> Train (vector<double> inputValues, vector<double> desiredOutput);
	vector<double> CalcOutput(vector<double> inputValues);
	void UpdateWeights(vector<double> outputs, vector<double> desiredOutput);
	double Sigmoid(double value);
	double ActivationFunction(double value);
	double ActivationFunctionO(double value);
	double TanH(double value);
	double ReLu(double value);
	double LeakyReLu(double value);
	void LoadWeights(int numberfile);
	void SaveWeights(int numberfile);
	FString PrintWeights();
	TArray<double> GetWeights();
	void Mutate();
	double P;
	double L;
	vector<NNLayer> layers = {};
private:
	int numInputs;
	int numOutputs;
	int numHidden;
	int numNeuronPerHidden;
	
};
