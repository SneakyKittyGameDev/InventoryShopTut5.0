// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryShopTutGameMode.h"
#include "InventoryShopTutCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInventoryShopTutGameMode::AInventoryShopTutGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
