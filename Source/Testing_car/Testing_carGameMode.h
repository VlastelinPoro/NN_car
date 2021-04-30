// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "Testing_carPawn.h"
#include "Testing_carGameMode.generated.h"

using namespace std;

UCLASS(minimalapi)
class ATesting_carGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATesting_carGameMode();
	virtual void BeginPlay() override;
	void Timer();
	void SpawnTrainedCar();
	void SpawnPopulation();
	void RemovePopulation();
	void InitAnn(int _population);
	void Selection();
	void Random_car();
	void Mutation();
	void CrossOver();
	void RemainderStochasticSampling();
	void Genetic_algorithm();
	void SavingStat();
	UFUNCTION(BlueprintCallable)
	int GetGeneration();
	UFUNCTION(BlueprintCallable)
	float GetAvScore();
	UFUNCTION(BlueprintCallable)
	int GetMaxScore();
	UFUNCTION(BlueprintCallable)
	bool GetLoadWeights();
	UFUNCTION(BlueprintCallable)
	float GetAvSpeed();
	UFUNCTION(BlueprintCallable)
	float GetAvDistance();
	UFUNCTION(BlueprintCallable)
	float GetMyDistance();
	UFUNCTION(BlueprintCallable)
	float GetMyAvSpeed();
	UFUNCTION(BlueprintCallable)
	float GetMyScore();
	UFUNCTION(BlueprintCallable)
	bool GetPlaying();
	UFUNCTION(BlueprintCallable)
	bool GetTraining();
	UFUNCTION(BlueprintCallable)
	float GetBestTime();
	void SaveWeights();
	TArray<ATesting_carPawn*> Sort(TArray<ATesting_carPawn*> arr);
	AActor *SpawnPoint = nullptr;
	TArray<ATesting_carPawn*> Cars_test;

	TArray<ANN*> Ann = {};

	TArray<ANN> Selected_ANN = {};


	UPROPERTY(EditAnywhere)
	bool Training = false;
	UPROPERTY(EditAnywhere)
	bool LoadWeights = false;
	UPROPERTY(EditAnywhere)
	bool Playing = false;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	float TraceDistance = 1000.0f;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	int epochs = 1000;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	int population = 20;
	UPROPERTY(EditAnywhere, Category="GA Settings")
	double MutProbability = 0.1;
	UPROPERTY(EditAnywhere, Category="GA Settings")
	double MutLimit = 0.1;
	UPROPERTY(EditAnywhere, Category="GA Settings")
	double CrossOverChance = 0.1;
	UPROPERTY(EditAnywhere, Category="GA Settings")
	int Num_best_car = 5;
	UPROPERTY(EditAnywhere, Category="GA Settings")
	int Num_new_random_car = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car Settings")
	TSubclassOf<class ATesting_carPawn> CarClass;

	float sum_score = 0;
	int time = 0; 
	int num_inputs = 7;
	int num_outputs = 2;
	int num_hidden  = 1;
	int num_neuron_per_hidden = 10;
	int generation = 1;
	int max_score;
	float best_time = 10000;
	ATesting_carPawn* best_car;
	TArray<FString> savingstats = {"Generation;Av. score;Max score;Av. speed;Av. distance"};
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};