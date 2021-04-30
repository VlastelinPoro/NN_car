// Fill out your copyright notice in the Description page of Project Settings.


#include "Check_Point.h"

ACheck_Point::ACheck_Point()
{
    OnActorBeginOverlap.AddDynamic(this, &ACheck_Point::AddScore);
}

void	ACheck_Point::AddScore(AActor *OverlappedActor, AActor *otherActor)
{
    ATesting_carPawn *car;

    car = dynamic_cast<ATesting_carPawn *>(otherActor);
    if (car == nullptr)
        return ;
    car->checkpoint += this->scoreToAdd;
    car->count_checkpoint += 1;
}
