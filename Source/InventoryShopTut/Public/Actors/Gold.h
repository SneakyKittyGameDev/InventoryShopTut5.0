// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Item.h"
#include "Gold.generated.h"

class AInventoryShopTutCharacter;

UCLASS()
class INVENTORYSHOPTUT_API AGold : public AItem
{
	GENERATED_BODY()
public:
	AGold();
	
	virtual void Use(AInventoryShopTutCharacter* Character, bool IsInShop = false) override;
};
