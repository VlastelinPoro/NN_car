#include "NNLayer.h"

NNLayer::NNLayer(int nNeurons, int numNeuronInputs)
{
    numNeurons = nNeurons;
    for(int i = 0 ; i<nNeurons;i++)
    {
        Neuron neuron(numNeuronInputs);
        neurons.push_back(neuron);
    }
}

NNLayer::~NNLayer()
{
}
