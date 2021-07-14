// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ShopKeeper.h"
#include "Actors/Item.h"

#include "Components/SkeletalMeshComponent.h"
#include "InventoryShopTut/InventoryShopTutCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShopKeeper::AShopKeeper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ShopKeeperMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = ShopKeeperMesh;

	bReplicates = true;
}

void AShopKeeper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShopKeeper, Items);
}

// Called when the game starts or when spawned
void AShopKeeper::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShopKeeper::OnRep_Items()
{
	if (AInventoryShopTutCharacter* Character = Cast<AInventoryShopTutCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (Character->IsLocallyControlled())
		{
			Character->OpenShop(this, Items);
		}
	}
}

void AShopKeeper::Interact(AInventoryShopTutCharacter* Character)
{
	if (Character)
	{
		Character->OpenShop(this, Items);
	}
}

void AShopKeeper::TransfferedItem(TSubclassOf<AItem> ItemSubclass)
{
	uint8 Index = 0;
	for (FItemData& Item : Items)
	{
		if (Item.ItemClass == ItemSubclass)
		{
			--Item.StackCount;
			if (Item.StackCount <= 0)
			{
				Items.RemoveAt(Index);
			}
			break;
		}
		++Index;
	}
	OnRep_Items();
}

bool AShopKeeper::CanBuyItem(int32 CurrentGold, TSubclassOf<AItem> ItemSubclass)
{
	for (FItemData& Item : Items)
	{
		if (Item.ItemClass == ItemSubclass)
		{
			return CurrentGold >= Item.ItemCost;
		}
	}
	return false;
}

bool AShopKeeper::BuyItem(AInventoryShopTutCharacter* Character, TSubclassOf<AItem> ItemSubclass)
{
	if (Character && ItemSubclass)
	{
		for (FItemData& Item : Items)
		{
			if (Item.ItemClass == ItemSubclass)
			{
				if (CanBuyItem(Character->GetCurrentGold(), ItemSubclass))
				{
					if (AItem* ItemCDO = ItemSubclass.GetDefaultObject())
					{
						ItemCDO->Use(Character, true);
						Character->RemoveGold(Item.ItemCost);
						TransfferedItem(ItemSubclass);
						return true;
					}
				}
			}
		}
	}
	return false;
}
