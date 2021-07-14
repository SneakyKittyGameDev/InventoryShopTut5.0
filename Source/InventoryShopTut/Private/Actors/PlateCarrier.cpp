// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlateCarrier.h"
#include "InventoryShopTut/InventoryShopTutCharacter.h"

APlateCarrier::APlateCarrier()
{
	bIsEquipable = true;
}

void APlateCarrier::Use(AInventoryShopTutCharacter* Character, bool IsInShop)
{
	if (Character)
	{
		AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spine_02"));
	}
}
