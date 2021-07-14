// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "Structs.h"
#include "ShopKeeper.generated.h"

UCLASS()
class INVENTORYSHOPTUT_API AShopKeeper : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopKeeper();

protected:
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* ShopKeeperMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Items, EditAnywhere, BlueprintReadWrite, Category = "TUTORIAL")
	TArray<FItemData> Items;
	UFUNCTION()
	void OnRep_Items();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void TransfferedItem(TSubclassOf<AItem> ItemSubclass);
	bool CanBuyItem(int32 CurrentGold, TSubclassOf<AItem> ItemSubclass);
	
public:
	virtual void Interact(class AInventoryShopTutCharacter* Character) override;
	bool BuyItem(class AInventoryShopTutCharacter* Character, TSubclassOf<AItem> ItemSubclass);
};
