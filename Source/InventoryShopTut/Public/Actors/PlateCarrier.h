// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item.h"
#include "PlateCarrier.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSHOPTUT_API APlateCarrier : public AItem
{
	GENERATED_BODY()

public:
	APlateCarrier();
	
	virtual void Use(class AInventoryShopTutCharacter* Character, bool IsInShop) override;	
};
