// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

void	AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAction("Spawn", IE_Pressed, this, &AMyPlayerController::Spawn);
    InputComponent->BindAction("Save", IE_Pressed, this, &AMyPlayerController::Save);
}

void	AMyPlayerController::Spawn()
{
    ATesting_carGameMode *gameMode;
    gameMode = (ATesting_carGameMode *)GetWorld()->GetAuthGameMode();
    if (!gameMode->Playing)
    {
        if (GetWorld())
        {
            gameMode->Genetic_algorithm();
        }
    }
}

void	AMyPlayerController::Save()
{
    ATesting_carGameMode *gameMode;
    gameMode = (ATesting_carGameMode *)GetWorld()->GetAuthGameMode();
    if (!gameMode->Playing)
    {
        if (GetWorld())
        {
            gameMode->SaveWeights();
        }
    }
}