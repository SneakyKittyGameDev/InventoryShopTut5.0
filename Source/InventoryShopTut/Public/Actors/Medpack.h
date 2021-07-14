// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item.h"
#include "Medpack.generated.h"


class AInventoryShopTutCharacter;

UCLASS()
class INVENTORYSHOPTUT_API AMedpack : public AItem
{
	GENERATED_BODY()
public:
	AMedpack();

protected:
	UPROPERTY(EditAnywhere)
	float HealthValue;

	
public:
	virtual void Use(AInventoryShopTutCharacter* Character, bool IsInShop = false) override;
};
