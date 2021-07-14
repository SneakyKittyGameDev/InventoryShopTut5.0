// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item.h"
#include "Food.generated.h"

class AInventoryShopTutCharacter;

UCLASS()
class INVENTORYSHOPTUT_API AFood : public AItem
{
	GENERATED_BODY()
public:
	AFood();
	
protected:
	UPROPERTY(EditAnywhere)
	float RemoveFoodValue;

public:
	virtual void Use(AInventoryShopTutCharacter* Character, bool IsInShop = false) override;
};
