// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Testing_carPawn.h"
#include "Check_Point.generated.h"


/**
 * 
 */
UCLASS()
class TESTING_CAR_API ACheck_Point : public ATriggerBox
{
	GENERATED_BODY()
	ACheck_Point();
	
	UFUNCTION()
    virtual void	AddScore(AActor *OverlappedActor, AActor *otherActor);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VNN")
	int	scoreToAdd = 100;
};
