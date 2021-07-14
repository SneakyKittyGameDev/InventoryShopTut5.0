// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryShopTut/Public/Structs.h"
#include "InventoryShopTut/Public/InteractableInterface.h"
#include "Item.generated.h"

UCLASS()
class INVENTORYSHOPTUT_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	UPROPERTY(EditDefaultsOnly, Category  = "TUTORIAL")
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category  = "TUTORIAL")
	FItemData ItemData;

	UPROPERTY(EditAnywhere, Category  = "TUTORIAL")
	bool bIsEquipable;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Interact(class AInventoryShopTutCharacter* Character) override;
	FItemData GetItemData() const { return ItemData; }

	virtual void Use(AInventoryShopTutCharacter* Character, bool IsInShop = false) override;

	bool IsEquipable() const { return bIsEquipable; }
};
