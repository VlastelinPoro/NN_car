// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Neuron.h"

/**
 * 
 */
class NNLayer
{
public:
	int numNeurons;
	vector<Neuron> neurons;
	NNLayer(int nNeurons, int numNeuronInputs);
	~NNLayer();
};
