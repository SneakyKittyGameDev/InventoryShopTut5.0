// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Medpack.h"
#include "InventoryShopTut/InventoryShopTutCharacter.h"

AMedpack::AMedpack()
{
	HealthValue = 10.0f;
	ItemData.ItemClass = StaticClass();
}

void AMedpack::Use(AInventoryShopTutCharacter* Character, bool IsInShop)
{
	Super::Use(Character, IsInShop);
	
	if (Character && !IsInShop)
	{
		Character->AddHealth(HealthValue);
	}
}
