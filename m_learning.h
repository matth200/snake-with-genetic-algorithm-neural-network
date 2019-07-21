/*
*	Réalisé par @matth200
*	Réseaux de neurone avec backprogagation
* 	la classe la plus importante est MachineLearning
*/

#ifndef LEARNING_DEFINE
#define LEARNING_DEFINE
 
#include <cmath>
#include <vector>

#include <cstdlib>

#include <ctime>
#include <iostream>

#include <fstream>

double sigmoid(double a);
double sigmoidPrime(double a);

class Neuron
{
public:
	Neuron(int size, bool israndom);
	void set_value(double v);
	double get_value() const;
	void set_weight(int i, double weight);
	double get_weight(int i) const;
	void set_bias(double bias);
	double get_bias() const;
	double get_error() const;
	void set_error(double error);
	int numberConnection() const;
protected:
	double value;
	double b;
	double deriError;
	std::vector<double> w;
};

class NetworkNeuron
{
public:
	NetworkNeuron(int size, NetworkNeuron *before);
	void set_after(NetworkNeuron *after);
	bool is_end() const;
	NetworkNeuron* getme();
	int get_number_neuron() const;
	Neuron* get_neuron(int index);
protected:
	std::vector<Neuron> neurons;
	NetworkNeuron *beforeNetwork, *afterNetwork;
};

class MachineLearning
{
public:
	MachineLearning();
	MachineLearning(int sizeInput);
	void open(int sizeInput);
	void setInput(char *data);
	void setInput(char *data, int size, int cursor);
	void setWeightRandom(int maxW = 2, int maxB = 2);
    void calcul();
    NetworkNeuron* getNetwork(int i);
	double getZ(int l, int j);	
	double getOutput(int index);
    int numberNeuronIn(int i);	
	void addColumn(int numberNeuron);
	int getNumberColumn() const; 
	double getPrecision(NetworkNeuron& result);
	void train(NetworkNeuron& result, double r = 0.5);
	void saveTraining(const char *file);
	void backupTraining(const char *file);
	int getPrediction();
protected:
	std::vector<NetworkNeuron> Lines;
};

#endif
