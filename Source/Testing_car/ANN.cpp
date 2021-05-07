#include "ANN.h"

#include <basetyps.h>
#include <fstream>
#include <string>
#include <Misc/FileHelper.h>
#include <HAL/PlatformFilemanager.h>
using namespace std;

ANN::ANN(): P(0.0), L(0.0), numInputs(0), numOutputs(0), numHidden(0), numNeuronPerHidden(0)
{
}

ANN::ANN(int _numInputs, int _numOutputs, int _numHidden, int _numNeuronPerHidden, double _P, double _L)
{
    numInputs = _numInputs;
    numOutputs = _numOutputs;
    numHidden = _numHidden;
    numNeuronPerHidden = _numNeuronPerHidden;
    P = _P;
    L = _L;

    if (numHidden>0)
    {
        layers.push_back(NNLayer(numNeuronPerHidden,numInputs));
        for (int i = 0; i<numHidden-1; i++)
        {
            layers.push_back(NNLayer(numNeuronPerHidden,numNeuronPerHidden));
        }
        layers.push_back(NNLayer(numOutputs,numNeuronPerHidden));
    }
    else
    {
        layers.push_back(NNLayer(numOutputs,numInputs));
    }
}


ANN::~ANN()
{
}

/*vector<double> ANN::Train(vector<double> inputValues, vector<double> desiredOutput)
{
    
    vector<double> outputs;
    outputs = CalcOutput(inputValues);
    UpdateWeights(outputs, desiredOutput);
    return outputs;
    
}*/

vector<double> ANN::CalcOutput(vector<double> inputValues)
{
    vector<double> inputs;
    vector<double> outputs;

    if (int(inputValues.size())!= numInputs)
    {
        cout << "Incorrect number of inputs" << endl;
        return outputs;
    }

    inputs = inputValues;
    for(int i = 0; i < numHidden + 1; i++)
    {
        if(i > 0)
        {
            inputs = outputs;
        }
        outputs.clear();

        for(int j = 0; j < layers[i].numNeurons;j++)
        {
            double N = 0;
            layers[i].neurons[j].inputs.clear();

            for(int k = 0; k < layers[i].neurons[j].numInputs; k++)
            {
                layers[i].neurons[j].inputs.push_back(inputs[k]);
                N += layers[i].neurons[j].weights[k]*inputs[k];
            }

            N-= layers[i].neurons[j].bias;
            if (i == numHidden)
            {
                layers[i].neurons[j].output = ActivationFunctionO(N);
            }
            else
            {
                layers[i].neurons[j].output = ActivationFunction(N);
            }
            outputs.push_back(layers[i].neurons[j].output);
        }
    }
    return outputs;
}

void ANN::UpdateWeights(vector<double> outputs, vector<double> desiredOutput)
{
    /*double error;
    for (int i = numHidden; i >= 0; i--)
    {
        for(int j = 0; j< layers[i].numNeurons; j++)
        {
            if(i == numHidden)
            {
                error = desiredOutput[j] - outputs[j];
                layers[i].neurons[j].errorGradient = outputs[j] * (1-outputs[j]) * error;
            }
            else
            {
                layers[i].neurons[j].errorGradient = layers[i].neurons[j].output * (1-layers[i].neurons[j].output);
                double errorGradSum = 0;
                for(int p = 0; p < layers[i+1].numNeurons; p++)
                {
                    errorGradSum += layers[i+1].neurons[p].errorGradient * layers[i+1].neurons[p].weights[j];
                }
                layers[i].neurons[j].errorGradient *= errorGradSum;
            }
            for(int k = 0; k < layers[i].neurons[j].numInputs; k++)
            {
                if(i == numHidden)
                {
                    error = desiredOutput[j] - outputs[j];
                    layers[i].neurons[j].weights[k] += alpha * layers[i].neurons[j].inputs[k] * error;
                }
                else
                {
                    layers[i].neurons[j].weights[k] += alpha * layers[i].neurons[j].inputs[k] * layers[i].neurons[j].errorGradient;
                }
            }
            layers[i].neurons[j].bias += alpha * -1 * layers[i].neurons[j].errorGradient;
        }
    }*/
}

double ANN::Sigmoid(double value)
{
    double k = (double) exp(value);
    return k / (1.0f + k);
}

double ANN::LeakyReLu(double value)
{
    if(value < 0) return 0.01*value;
    else return value;
}


vector<FString> ANN::Split(FString line, FString delit)
{
    vector<FString> splited_line;
    FString buffer = "";

    while(line.Split(delit,&buffer,&line,ESearchCase::IgnoreCase, ESearchDir::FromStart))
    {
        splited_line.push_back(buffer);
    }
    splited_line.push_back(line);
    return splited_line;
}

void ANN::LoadWeights(int numberfile)
{
    FString path = FPaths::ProjectDir();
    FString filename = "weights";
    filename += FString::FromInt(numberfile) + ".txt";
    path += filename;
    FString weightStr = "";
    FFileHelper::LoadFileToString(weightStr, *path);
    vector<FString> weightValues = Split(weightStr,",");
    int w = 0;
    for(NNLayer &l : layers)
    {
        for(Neuron &n : l.neurons)
        {
            for(int i = 0; i < n.weights.size(); i++)
            {
                n.weights[i] = FCString::Atof(*weightValues[w]);
                w++;
            }
            n.bias = FCString::Atof(*weightValues[w]); 
            w++;
        }
    }
}

void ANN::SaveWeights(int numberfile)
{
    FString SaveDirectory = FPaths::ProjectDir();
    FString filename = "weights";
    filename += FString::FromInt(numberfile) + ".txt";
    SaveDirectory += filename;
    FString weightStr = "";
    for(NNLayer l : layers)
    {
        for(Neuron n : l.neurons)
        {
            for(double w : n.weights)
            {
                weightStr.Append(FString::SanitizeFloat(w));
                weightStr.Append(",");
            }
            weightStr.Append(FString::SanitizeFloat(n.bias));
            weightStr.Append(",");
        }
    }
    FFileHelper::SaveStringToFile(weightStr, *SaveDirectory);
}

FString ANN::PrintWeights()
{
    ofstream fout("weights.txt");
    FString weightStr = "";
    for(NNLayer l : layers)
    {
        for(Neuron n : l.neurons)
        {
            for(double w : n.weights)
            {
                weightStr.Append(FString::SanitizeFloat(w));
                weightStr.Append(",");
            }
            weightStr.Append(FString::SanitizeFloat(n.bias));
            weightStr.Append(",");
        }
    }
    return weightStr;
}

TArray<double> ANN::GetWeights()
{
    TArray<double> Weights = {};
    for(NNLayer l : layers)
    {
        for(Neuron n : l.neurons)
        {
            for(double w : n.weights)
            {
                Weights.Add(w);
            }
            Weights.Add(n.bias);
        }
    }
   
    return Weights;
}

void ANN::Mutate()
{
    for (int i = numHidden; i >= 0; i--)
    {
        for(int j = 0; j< layers[i].numNeurons; j++)
        {
            for(int k = 0; k < layers[i].neurons[j].numInputs; k++)
            {
                double X1 = (rand() % 100) / 100.0;
                double X2 = (rand() % 100) / 100.0;
                if (X1 < P)
                {
                    layers[i].neurons[j].weights[k] += (2*X2 - 1)*L;
                }
            }
            double X1 = (rand() % 100) / 100.0;
            double X2 = (rand() % 100) / 100.0;
            if (X1 < P)
            {
                layers[i].neurons[j].bias +=  (2*X2 - 1)*L;
            }
        }
    }
}

double ANN::ReLu(double value)
{
    if(value > 0) return value;
    else return 0;
}

double ANN::TanH(double value)
{
    double k = (double) exp(-2*value);
    return 2 / (1.0f + k) - 1;
}

double ANN::ActivationFunctionO(double value)
{
    return TanH(value);
}

double ANN::ActivationFunction(double value)
{
    return TanH(value);
}
