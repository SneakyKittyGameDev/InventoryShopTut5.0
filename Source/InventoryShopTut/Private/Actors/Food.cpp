// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Food.h"
#include "InventoryShopTut/InventoryShopTutCharacter.h"

AFood::AFood()
{
	RemoveFoodValue = 30.0f;
	ItemData.ItemClass = StaticClass();
}

void AFood::Use(AInventoryShopTutCharacter* Character, bool IsInShop)
{
	Super::Use(Character, IsInShop);
	
	if (Character && !IsInShop)
	{
		Character->RemoveHunger(RemoveFoodValue);
	}
}
