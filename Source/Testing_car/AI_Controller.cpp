// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Controller.h"


// Sets default values
AAI_Controller::AAI_Controller()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}


void AAI_Controller::SpawnPopulation()
{
	for(int i = 0; i < population; i++)
	{
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();
		FTransform transform(Rotation, Location);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATesting_carPawn *car  = GetWorld()->SpawnActor<ATesting_carPawn>(CarClass, Location, Rotation, SpawnParams);
		car->epochs = this->epochs;
		car->TraceDistance = this->TraceDistance;
		Cars_test.Add(car);
		Cars_test[i]->SpawnDefaultController();
		Cars_test[i]->Dead=false;
		Cars_test[i]->SetActorTickEnabled(true);
		Cars_test[i]->ann = ANN(5,2,1,10,0.05,0.1);
		//Cars_test[i]->GetMesh()->SetNotifyRigidBodyCollision(true);
	}
}

void AAI_Controller::RemovePopulation()
{
	for(int i = 0; i < population; i++)
	{
		Cars_test[i]->Destroy();
	}
	Cars_test.RemoveAt(0, population);
}

// Called when the game starts or when spawned
void AAI_Controller::BeginPlay()
{
	Super::BeginPlay();
	OurPlayer = UGameplayStatics::GetPlayerController(this, 0);
	//SpawnPopulation();

}

// Called every frame
void AAI_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	int counter=0;
	
	/*for(int i = 0; i < population; i++)
	{
		if (Cars_test[i]->Dead==true)
		{
			counter++;
		}
	}
	if (counter == population)
	{
		RemovePopulation();
		SpawnPopulation();
	}*/
}

