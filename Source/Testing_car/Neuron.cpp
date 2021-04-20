// Fill out your copyright notice in the Description page of Project Settings.


#include "Neuron.h"

Neuron::Neuron(int _numInputs): output(0), errorGradient(0)
{
    bias = (-100 + rand() % ((100 + 1) + 100)) / 100.0;
    numInputs = _numInputs;
    for (int i = 0; i < numInputs; i++)
    {
        weights.push_back((-100 + rand() % ((100 + 1) + 100)) / 100.0);
    }
}

Neuron::~Neuron()
{
}
