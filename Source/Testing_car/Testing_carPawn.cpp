#include "Testing_carPawn.h"
#include "Testing_carWheelFront.h"
#include "Testing_carWheelRear.h"
#include "Testing_carHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DataTable.h"
#include "Materials/Material.h"


// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "DrawDebugHelpers.h"
#endif // HMD_MODULE_INCLUDED

const FName ATesting_carPawn::LookUpBinding("LookUp");
const FName ATesting_carPawn::LookRightBinding("LookRight");



#define LOCTEXT_NAMESPACE "VehiclePawn"

ATesting_carPawn::ATesting_carPawn()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));

	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	
	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UTesting_carWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UTesting_carWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UTesting_carWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UTesting_carWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);
	
	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;
	Camera->Activate();

	/*
	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);
	*/
	bInReverseGear = false;
	
	CollisionParams.AddIgnoredActor(this);
	
}

void ATesting_carPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATesting_carPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATesting_carPawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");
	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ATesting_carPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ATesting_carPawn::OnHandbrakeReleased);
}


void ATesting_carPawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ATesting_carPawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ATesting_carPawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void ATesting_carPawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}


float ATesting_carPawn::Map(float nf, float nt, float of, float ot, float val)
{
	if (val <= of)
		return nf;
	else if (val >= ot)
		return nt;
	return (nt - nf) * ((val - of) / (ot - of)) + nf;	
}

void ATesting_carPawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	this->Dead = true;
	GetMesh()->SetMaterial(2, mat_dead);
	//GEngine->AddOnScreenDebugMessage(0,5.0f,FColor::Green, this->ann.PrintWeights());
	//GEngine->AddOnScreenDebugMessage(1,5.0f,FColor::Green, FString::SanitizeFloat(this->score));
}

vector<FString> ATesting_carPawn::Split(FString line, FString delit)
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


void ATesting_carPawn::LoadTrainingSet()
{
	/*
	FString path = FPaths::ProjectDir() + "/trainingData.txt";
	vector<double> calcOutputs = {};
	vector<double> inputs = {};
	vector<double> outputs = {};
	TArray<FString> lines;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	if(file.FileExists(*path))
	{
		FFileHelper::LoadFileToStringArray(lines, *path);
		
		for (int i = 0 ; i < epochs; i++)
		{
			sse = 0;
			FString currentWeights = ann.PrintWeights();
			
			for(int j=0;j<lines.Num();j++)
			{
				vector<FString> data = Split(lines[j],",");
				float thisError = 0;
				float test = FCString::Atof(*data[5]);
				float test_1 = FCString::Atof(*data[6]);
				if ((test != 0) && (test_1 != 0))
				{
					inputs.clear();
					outputs.clear();
					inputs.push_back(FCString::Atof(*data[0]));
					inputs.push_back(FCString::Atof(*data[1]));
					inputs.push_back(FCString::Atof(*data[2]));
					inputs.push_back(FCString::Atof(*data[3]));
					inputs.push_back(FCString::Atof(*data[4]));

					double o1 = Map(0,1,-1,1,FCString::Atof(*data[5]));
					outputs.push_back(o1);
					double o2 = Map(0,1,-1,1,FCString::Atof(*data[6]));
					outputs.push_back(o2);

					calcOutputs = ann.Train(inputs,outputs);
					thisError = (pow((float)outputs[0]-calcOutputs[0],2)
						+ pow((float)outputs[1]-calcOutputs[1],2))/2.0f;
				}
				sse+=thisError;
			}
			trainingProgress = (float)i/(float)epochs;
			GEngine->AddOnScreenDebugMessage(2,10.0f,FColor::Green, FString::SanitizeFloat(trainingProgress));
			sse /= lines.Num();

			if(lastSSE < sse)
			{
				ann.LoadWeights(currentWeights);
				ann.alpha = FMath::Clamp((float)ann.alpha - 0.001f,0.01f,0.9f);
			}
			else //increase alpha
			{
				ann.alpha = FMath::Clamp((float)ann.alpha + 0.001f,0.01f,0.9f);
				lastSSE = sse;
			}
			
			GEngine->AddOnScreenDebugMessage(1,10.0f,FColor::Green, FString::SanitizeFloat(lastSSE));	
		}
		trainingDone = true;
	}
 */
}

void ATesting_carPawn::settime()
{
	_time+=0.1f;
}

void ATesting_carPawn::BeginPlay()
{
	Super::BeginPlay();
	//LoadTrainingSet();
	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->OnComponentHit.AddDynamic(this, &ATesting_carPawn::OnHit);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ATesting_carPawn::settime, 0.1f, true);
}


void ATesting_carPawn::Tick(float Delta)
{
	Super::Tick(Delta);
	if (count_checkpoint == 52)
	{
		lap_time = _time;
		_time = 0;
		count_checkpoint = 0;
	}
	
	if (!Dead)
	{
		car_speed = GetVehicleMovement()->GetForwardSpeed()/10;
		counter++;
		car_distance += car_speed *  Delta;
		sum_speed += car_speed;
		av_speed = sum_speed/counter;
		score =  checkpoint + av_speed + best_time_reward; //Fitness function of the car
		//score = car_distance/10 + checkpoint + av_speed; //Fitness function of the car
		//score = car_distance/10 + checkpoint + av_speed; //Fitness function of the car
		//score = car_distance/10 + checkpoint + av_speed; //Fitness function of the car
		if(!playing)
		{
			vector<double> calcOutputs = {};
			vector<double> inputs = {};
			//GetVehicleMovementComponent()->SetThrottleInput(0.5);
			
			// Setup the flag to say we are in reverse gear
			bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
			
			// Update the strings used in the hud (incar and onscreen)
			UpdateHUDStrings();

			// Set the string in the incar hud
			SetupInCarHUD();
			
			FHitResult outhit;

			FVector Start = GetActorLocation();
			FRotator actorrot = GetActorRotation();
			FRotator actorrot45 = GetActorRotation();;
			FRotator actorrot90 = GetActorRotation(); ;
			FRotator actorrotm45 = GetActorRotation();;
			FRotator actorrotm90 = GetActorRotation();;
			FRotator actorrot30 = GetActorRotation();
			FRotator actorrotm30 = GetActorRotation();
			FRotator actorrot60 = GetActorRotation();
			FRotator actorrotm60 = GetActorRotation();
			FRotator actorrot80 = GetActorRotation();
			FRotator actorrotm80 = GetActorRotation();
			Start.Z = Start.Z + 150;	
			
			CollisionParams.AddIgnoredActor(this);
			actorrot45.Yaw= actorrot.Yaw + 45;
			actorrotm45.Yaw= actorrot.Yaw - 45;
			actorrot90.Yaw= actorrot.Yaw + 90;
			actorrotm90.Yaw= actorrot.Yaw - 90;
			actorrot30.Yaw = actorrot.Yaw + 30;
			actorrotm30.Yaw =  actorrot.Yaw - 30;
			actorrot60.Yaw =  actorrot.Yaw + 60;
			actorrotm60.Yaw =  actorrot.Yaw - 60;
			actorrot80.Yaw =  actorrot.Yaw + 10;
			actorrotm80.Yaw =  actorrot.Yaw - 10;
			


			double distance, distance_90, distance_90m, distance_30, distance_30m;
			double distance_60, distance_60m, distance_45, distance_45m, distance_10, distance_10m;
			
			FVector End = actorrot.Vector()*TraceDistance+ Start;
			FVector End_45 =actorrot45.Vector() * TraceDistance + Start;
			FVector End_90 =actorrot90.Vector() * TraceDistance + Start;
			FVector End_m45 =actorrotm45.Vector() * TraceDistance + Start;
			FVector End_m90 =actorrotm90.Vector() * TraceDistance + Start;
			FVector End_30 =actorrot30.Vector() * TraceDistance + Start;
			FVector End_m30 =actorrotm30.Vector() * TraceDistance + Start;
			FVector End_60 =actorrot60.Vector() * TraceDistance + Start;
			FVector End_m60 =actorrotm60.Vector() * TraceDistance + Start;
			FVector End_10 =actorrot80.Vector() * TraceDistance + Start;
			FVector End_m10 =actorrotm80.Vector() * TraceDistance + Start;
			
			//DrawDebugLine(GetWorld(),Start,End,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_45,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_90,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_m45,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_m90,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_30,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_m30,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_60,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_m60,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_80,FColor::Green, false, -1, 0 ,2);
			//DrawDebugLine(GetWorld(),Start,End_m80,FColor::Green, false, -1, 0 ,2);

			bool IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance =  1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance);
			}
			
			
			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_45, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_45 = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_45);
			}
			
			
			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_90, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_90 =  1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_90);	
			}
			
			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_m45, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_45m = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_45m);	
			}
			
			
			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_m90, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_90m = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_90m);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_30, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_30 = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_30);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_m30, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_30m = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_30m);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_60, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_60 = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_60);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_m60, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_60m = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_60m);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_10, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_10 = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_10);
			}

			IsHit = GetWorld()->LineTraceSingleByChannel(outhit, Start, End_m10, ECollisionChannel::ECC_Vehicle, CollisionParams);
			if(IsHit)
			{
				distance_10m = 1 - outhit.Distance/TraceDistance;
				FString Dist = FString::SanitizeFloat(distance_10m);
			}
			
			//inputs.push_back(distance);
			inputs.push_back(distance_90);
			inputs.push_back(distance_90m);
			//inputs.push_back(distance_30m);
			//inputs.push_back(distance_30);
			inputs.push_back(distance_60m);
			inputs.push_back(distance_60);
			inputs.push_back(distance_10m);
			inputs.push_back(distance_10);
			//inputs.push_back(distance_45);
			//inputs.push_back(distance_45m);

			
			
			inputs.push_back(car_speed * 0.36f / 1000);
			calcOutputs = ann.CalcOutput(inputs);
			//float translationInput = Map(-1,1,0,1,(float) calcOutputs[0]);
			//float rotationInput = Map(-1,1,0,1,(float) calcOutputs[1]);
			float translationInput = calcOutputs[0];
			float rotationInput = calcOutputs[1];
			//float break_car = calcOutputs[2];
			//GEngine->AddOnScreenDebugMessage(3,0.0f,FColor::Green, FString::SanitizeFloat(translationInput));
			//GEngine->AddOnScreenDebugMessage(4,0.0f,FColor::Green, FString::SanitizeFloat(rotationInput));
			
			GetVehicleMovementComponent()->SetThrottleInput(translationInput);
			GetVehicleMovementComponent()->SetSteeringInput(rotationInput);

			/*
			if (break_car>0)
				GetVehicleMovementComponent()->SetHandbrakeInput(true);
			else
				GetVehicleMovementComponent()->SetHandbrakeInput(false);*/
			//GEngine->AddOnScreenDebugMessage(1,5,FColor::Green, FString::SanitizeFloat(GetVehicleMovement()->GetForwardSpeed() * 0.036f));
			//GEngine->AddOnScreenDebugMessage(2,5,FColor::Green, FString::SanitizeFloat(car_distance));
			//GEngine->AddOnScreenDebugMessage(2,5,FColor::Green, FString::SanitizeFloat(counter));
			if (counter > 300 && score<5)
			{
				this->Dead = true;
				GetMesh()->SetMaterial(2, mat_dead);
			}
			if (car_speed<0.01 && counter>500)
			{
				this->Dead = true;
				GetMesh()->SetMaterial(2, mat_dead);
			}
		}
	}
	else
	{
		GetVehicleMovementComponent()->SetThrottleInput(0);
		GetVehicleMovementComponent()->SetSteeringInput(0);
		
	}
}	

void ATesting_carPawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void ATesting_carPawn::SetupInCarHUD()
{
	//APlayerController* PlayerController = Cast<APlayerController>(GetController());
	/*if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}*/
}

void ATesting_carPawn::UpdatePhysicsMaterial()
{
}


#undef LOCTEXT_NAMESPACE
