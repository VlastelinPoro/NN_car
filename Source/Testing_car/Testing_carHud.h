// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "Testing_carHud.generated.h"


UCLASS(config = Game)
class ATesting_carHud : public AHUD
{
	GENERATED_BODY()

public:
	ATesting_carHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
