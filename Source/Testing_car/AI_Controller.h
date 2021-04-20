// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Testing_carPawn.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AI_Controller.generated.h"

class UInputComponent;

UCLASS()
class TESTING_CAR_API AAI_Controller : public AActor
{
	GENERATED_BODY()
	
public:
	
	APlayerController* OurPlayer;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	float TraceDistance = 1000.0f;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	int epochs = 1000;
	UPROPERTY(EditAnywhere, Category="Car Settings")
	int population = 20;
	UPROPERTY(EditAnywhere, Category="Player")
	bool player = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car Settings")
	TSubclassOf<class ATesting_carPawn> CarClass;
	// Sets default values for this actor's properties
	AAI_Controller();
	TArray<ATesting_carPawn*> Cars_test;


	UFUNCTION(BlueprintCallable)
	void SpawnPopulation();
	void RemovePopulation();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
