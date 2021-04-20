#include "Testing_carGameMode.h"
#include "EngineUtils.h"
#include "Testing_carPawn.h"
#include "Testing_carHud.h"
#include "Kismet/GameplayStatics.h"
#include <fstream>
#include <string>


ATesting_carGameMode::ATesting_carGameMode()
{
	DefaultPawnClass = ATesting_carPawn::StaticClass();
	HUDClass = ATesting_carHud::StaticClass();

}

void ATesting_carGameMode::BeginPlay()
{
	Super::BeginPlay();
	TActorIterator<AActor> ActorItr = TActorIterator<AActor>(GetWorld());

	while (ActorItr)
	{
		FString name = ActorItr->GetName();
		if (name == "MyAI_Controller_3" && GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("SpawnPoint found")));
			SpawnPoint = *ActorItr;
		}
		++ActorItr;
	}
	if (Training)
	{
		if (LoadWeights==false)
		{
			InitAnn(population);
			SpawnPopulation();
		}
		else
		{
			InitAnn(population);
			for(int i = 0; i < Num_best_car; i++)
			{
				Ann[i]->LoadWeights(i);
				Selected_ANN.Add(*Ann[i]);
			}
			CrossOver();
			Mutation();
			Random_car();
			SpawnPopulation();
		}
	}
	else
	{
		if (LoadWeights==true)
		{
			InitAnn(Num_best_car);
			SpawnTrainedCar();
		}
	}
}


void  ATesting_carGameMode::SpawnTrainedCar()
{
	auto OurPlayer = UGameplayStatics::GetPlayerController(this, 0);
	for(int i = 0; i < Num_best_car; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATesting_carPawn *car  = GetWorld()->SpawnActor<ATesting_carPawn>(CarClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), SpawnParams);
		car->epochs = this->epochs;
		car->TraceDistance = this->TraceDistance;
		Cars_test.Add(car);
		Cars_test[i]->SpawnDefaultController();
		Cars_test[i]->Dead=false;
		Cars_test[i]->SetActorTickEnabled(true);
		Ann[i]->LoadWeights(i);
		GEngine->AddOnScreenDebugMessage(i,5,FColor::Green, Ann[i]->PrintWeights());
		Cars_test[i]->ann = *Ann[i];
		Cars_test[i]->score = 0;
	}
	if (Playing)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATesting_carPawn *car  = GetWorld()->SpawnActor<ATesting_carPawn>(CarClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), SpawnParams);
		car->epochs = this->epochs;
		car->TraceDistance = this->TraceDistance;
		Cars_test.Add(car);
		Cars_test[Num_best_car]->SpawnDefaultController();
		Cars_test[Num_best_car]->Dead=false;
		Cars_test[Num_best_car]->SetActorTickEnabled(true);
		Cars_test[Num_best_car]->score = 0;
		Cars_test[Num_best_car]->playing = true;
		OurPlayer->UnPossess();
		OurPlayer->Possess(Cars_test[Num_best_car]);
	}
	
}

void ATesting_carGameMode::SpawnPopulation()
{
	for(int i = 0; i < population; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATesting_carPawn *car  = GetWorld()->SpawnActor<ATesting_carPawn>(CarClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), SpawnParams);
		car->epochs = this->epochs;
		car->TraceDistance = this->TraceDistance;
		Cars_test.Add(car);
		Cars_test[i]->SpawnDefaultController();
		Cars_test[i]->Dead=false;
		Cars_test[i]->SetActorTickEnabled(true);
		Cars_test[i]->ann = *Ann[i];
		Cars_test[i]->score = 0;
		//Cars_test[i]->GetMesh()->SetNotifyRigidBodyCollision(true);
	}
}

void ATesting_carGameMode::RemovePopulation()
{
	for(int i = 0; i < population; i++)
	{
		Cars_test[i]->Destroy();
	}
	Cars_test.RemoveAt(0, population);
}

void ATesting_carGameMode::InitAnn(int _population)
{
	for(int i = 0; i < _population; i++)
	{
		ANN *ann_1 = new ANN(num_inputs,num_outputs,num_hidden,num_neuron_per_hidden,MutProbability,MutLimit);
		Ann.Add(ann_1);
	}
}

TArray<ATesting_carPawn*> ATesting_carGameMode::Sort(TArray<ATesting_carPawn*> arr)
{
	for (int i = 0; i < population - 1; i++) {
		for (int j = i; j < population - i - 1; j++) {
			if (arr[i]->score < arr[j + 1]->score) {
				auto temp = arr[i];
				arr[i] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	return arr;
}

//Выбор лучших нейронок у тачек
void ATesting_carGameMode::Selection()
{
	int count=0;
	Selected_ANN.Empty();
	Cars_test = Sort(Cars_test);
	for(int i = 0; i<Num_best_car; i++)
	{
		Selected_ANN.Add(Cars_test[i]->ann);
		Cars_test[i]->ann.SaveWeights(i);
		//GEngine->AddOnScreenDebugMessage(++count,5.0f,FColor::Green, Cars_test[i]->ann.PrintWeights());
		//GEngine->AddOnScreenDebugMessage(++count,5.0f,FColor::Green, FString::SanitizeFloat(Cars_test[i]->score));
	}
	RemainderStochasticSampling();
		
}

void ATesting_carGameMode::Random_car()
{
	for(int i =population-Num_new_random_car; i < population; i++)
	{
		Ann[i] = new ANN(num_inputs, num_outputs,num_hidden, num_neuron_per_hidden,MutProbability,MutLimit);
	}
}
void ATesting_carGameMode::Mutation()
{
	for(int i = Num_best_car; i < population-Num_new_random_car; i++)
	{
		Ann[i]->Mutate();	
	}
	/*for(int i = 0; i < population-Num_new_random_car; i++)
	{
		Ann[i]->Mutate();	
	}*/
}

void ATesting_carGameMode::CrossOver()
{
	for (int i  = 0; i < Num_best_car; i++)
		*Ann[i] = Selected_ANN[i];
	for(int c = 0; c < population-Num_new_random_car; c++)
	{
		int random_index_selected_ann_1 = (rand() % (Selected_ANN.Num()));
		int random_index_selected_ann_2 = (rand() % (Selected_ANN.Num()));
		//FString weight_before = Ann[c]->PrintWeights();
		//FString weight_before_1 = Selected_ANN[random_index_selected_ann_1].PrintWeights();
		//FString weight_before_2 = Selected_ANN[random_index_selected_ann_2].PrintWeights();
		for (int i = num_hidden; i >= 0; i--)
		{
			for(int j = 0; j< Ann[c]->layers[i].numNeurons; j++)
			{
				for(int k = 0; k < Ann[c]->layers[i].neurons[j].numInputs; k++)
				{
					if ((rand() % 100) / 100.0 < CrossOverChance)
						Ann[c]->layers[i].neurons[j].weights[k] = Selected_ANN[random_index_selected_ann_1].layers[i].neurons[j].weights[k];
					else
						Ann[c]->layers[i].neurons[j].weights[k] = Selected_ANN[random_index_selected_ann_2].layers[i].neurons[j].weights[k];
				}
				if ((rand() % 100) / 100.0 < CrossOverChance)
					Ann[c]->layers[i].neurons[j].bias  = Selected_ANN[random_index_selected_ann_1].layers[i].neurons[j].bias;
				else
					Ann[c]->layers[i].neurons[j].bias = Selected_ANN[random_index_selected_ann_2].layers[i].neurons[j].bias;
			}
		}
		FString weight_after = Ann[c]->PrintWeights();
	}
}


void ATesting_carGameMode::RemainderStochasticSampling()
{
	//Selected_ANN.Empty();
	//Cars_test = Sort(Cars_test);
	float sum_score_best = 0;
	for (int i = 0; i<Num_best_car;i++)
	{
		sum_score_best += Cars_test[i]->score;
	}
	for (int i = 0; i<Num_best_car;i++)
	{
		Cars_test[i]->probability = Cars_test[i]->score/sum_score_best;
	}

	for (int i = 0; i<Num_best_car && Selected_ANN.Num()<population;i++)
	{
		int number = floor(Cars_test[i]->probability*population);
		for (int j = 0; j<number && Selected_ANN.Num()<population;j++)
		{
			Selected_ANN.Add(Cars_test[i]->ann);
		}
	}
}

void ATesting_carGameMode::Genetic_algorithm()
{
	SavingStat();
	max_score = 0;
	//RemainderStochasticSampling();
	Selection();
	CrossOver();
	Mutation();
	Random_car();
	RemovePopulation();
	SpawnPopulation();
	generation++;
}

void ATesting_carGameMode::SavingStat()
{
	FString SaveDirectory = FPaths::ProjectDir();
	FString filename = "stats.csv";
	SaveDirectory += filename;
	savingstats.Add(FString::FromInt(generation) + ';' + FString::SanitizeFloat(GetAvScore()) + ';' +
					FString::SanitizeFloat(GetMaxScore()) + ';' + FString::SanitizeFloat(GetAvSpeed()) + ';' +
					FString::SanitizeFloat(GetAvDistance()));
	FString FinalString;
	for (FString &Each : savingstats)
	{
		FinalString += Each;
		FinalString += LINE_TERMINATOR;
	}
	FFileHelper::SaveStringToFile(FinalString, *SaveDirectory);
}

int ATesting_carGameMode::GetGeneration()
{
	return generation;
}

float ATesting_carGameMode::GetAvScore()
{
	sum_score = 0;
	for(int i = 0; i < population; i++)
	{
		sum_score += Cars_test[i]->score;
		if (Cars_test[i]->score > max_score)
			max_score = Cars_test[i]->score;
	}
	return sum_score/population;
}

int ATesting_carGameMode::GetMaxScore()
{
	return max_score;
}

bool ATesting_carGameMode::GetLoadWeights()
{
	return LoadWeights;
}

float ATesting_carGameMode::GetAvSpeed()
{
	float av_speed = 0;
	for(int i = 0; i < population; i++)
	{
		av_speed += Cars_test[i]->av_speed;
	}
	return av_speed/population;
}

float ATesting_carGameMode::GetAvDistance()
{
	float av_dist = 0;
	for(int i = 0; i < population; i++)
	{
		av_dist += Cars_test[i]->car_distance;
	}
	return av_dist/population;
}

float ATesting_carGameMode::GetMyDistance()
{
	return Cars_test[Num_best_car]->car_distance;
}

float ATesting_carGameMode::GetMyAvSpeed()
{
	return Cars_test[Num_best_car]->av_speed;
}

float ATesting_carGameMode::GetMyScore()
{
	return Cars_test[Num_best_car]->score;
}

bool ATesting_carGameMode::GetPlaying()
{
	return Playing;
}

bool ATesting_carGameMode::GetTraining()
{
	return Training;
}

void ATesting_carGameMode::SaveWeights()
{
	//Selection();
	for(int i = 0; i<Num_best_car; i++)
	{
		//GEngine->AddOnScreenDebugMessage(i,5.0f,FColor::Green, Selected_ANN[i].PrintWeights());
		Selected_ANN[i].SaveWeights(i);
	}
	GEngine->AddOnScreenDebugMessage(0,5.0f,FColor::Green, "Saved");
}
void ATesting_carGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Training)
	{
		int counter=0;
		for(int i = 0; i < population; i++)
		{
			if (Cars_test[i]->Dead==true)
				counter++;
			//GEngine->AddOnScreenDebugMessage(i,0.0f,FColor::Green, FString::SanitizeFloat(Cars_test[i]->score));
		}
	
		if (counter == population)
		{
			Genetic_algorithm();
		}
	}
	if (Playing)
	{
		for(int i = 0; i <= Num_best_car; i++)
		{
			//GEngine->AddOnScreenDebugMessage(i,5,FColor::Green, FString::SanitizeFloat(Cars_test[i]->score));
		}
	}
}
