// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryShopTutCharacter.h"
#include "Actors/ShopKeeper.h"
#include "InventoryShopTut/Public/Actors/Item.h"
#include "InventoryShopTut/Public/Actors/Gold.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AInventoryShopTutCharacter::AInventoryShopTutCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AInventoryShopTutCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AInventoryShopTutCharacter::Interact);

	PlayerInputComponent->BindAxis("MoveForward", this, &AInventoryShopTutCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AInventoryShopTutCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AInventoryShopTutCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AInventoryShopTutCharacter::LookUpAtRate);
}

void AInventoryShopTutCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AInventoryShopTutCharacter, InventoryItems, COND_OwnerOnly);
	DOREPLIFETIME(AInventoryShopTutCharacter, Health);
	DOREPLIFETIME(AInventoryShopTutCharacter, Hunger);
}

void AInventoryShopTutCharacter::OnRep_Stats()
{
	if (IsLocallyControlled())
	{
		UpdateStats(Hunger, Health);
	}
}

void AInventoryShopTutCharacter::OnRep_InventoryItems()
{
	if (InventoryItems.Num())
	{
		AddItemAndUpdateInventoryWidget(InventoryItems[InventoryItems.Num() - 1], InventoryItems);
	}
	else
	{
		AddItemAndUpdateInventoryWidget(FItemData(), InventoryItems);
	}
}

int32 AInventoryShopTutCharacter::GetCurrentGold()
{
	for (FItemData& Item : InventoryItems)
	{
		if (Item.ItemClass == AGold::StaticClass())
		{
			return Item.StackCount;
		}
	}
	return 0;
}

void AInventoryShopTutCharacter::RemoveGold(int32 AmountToRemove)
{
	UseRemoveItem(AGold::StaticClass(), false, AmountToRemove);
}

void AInventoryShopTutCharacter::AddInventoryItem(FItemData ItemData)
{
	if (HasAuthority())
	{
		bool bIsNewItem = true;
		for (FItemData& Item : InventoryItems)
		{
			if (Item.ItemClass == ItemData.ItemClass)
			{
				if (ItemData.StackCount > 1)
				{
					Item.StackCount += ItemData.StackCount;
				}
				else
				{
					++Item.StackCount;
				}
				bIsNewItem = false;
				break;
			}
		}
		if (bIsNewItem)
		{
			InventoryItems.Add(ItemData);
		}
		if (IsLocallyControlled())
		{
			OnRep_InventoryItems();
		}
	}
}

void AInventoryShopTutCharacter::AddHealth(float Value)
{
	Health += Value;
	if (IsLocallyControlled())
	{
		UpdateStats(Hunger, Health);
	}
}

void AInventoryShopTutCharacter::RemoveHunger(float Value)
{
	Hunger -= Value;
	if (IsLocallyControlled())
	{
		UpdateStats(Hunger, Health);
	}
}

void AInventoryShopTutCharacter::UseItem(TSubclassOf<AItem> ItemSubclass, AShopKeeper* ShopKeeper, bool IsShopItem)
{
	if (ItemSubclass)
	{
		if (HasAuthority())
		{
			if (!ShopKeeper)
			{
				UseRemoveItem(ItemSubclass, true);
			}
			else
			{
				ShopKeeper->BuyItem(this, ItemSubclass);
				if (IsLocallyControlled())
				{
					OnRep_InventoryItems();
				}
			}
		}
		else
		{
			if (ShopKeeper)
			{
				if (ShopKeeper->BuyItem(this, ItemSubclass))
				{
					OnRep_InventoryItems();
				}
				else
				{
					return;
				}
			}
			if (AItem* Item = ItemSubclass.GetDefaultObject())
			{
				Item->Use(this, IsShopItem);
			}
			Server_UseItem(ItemSubclass, ShopKeeper, IsShopItem);
		}
	}
}

bool AInventoryShopTutCharacter::Server_UseItem_Validate(TSubclassOf<AItem> ItemSubclass, AShopKeeper* ShopKeeper, bool IsShopItem)
{
	return true;
}

void AInventoryShopTutCharacter::Server_UseItem_Implementation(TSubclassOf<AItem> ItemSubclass, AShopKeeper* ShopKeeper, bool IsShopItem)
{
	if (IsShopItem)
	{
		UseItem(ItemSubclass, ShopKeeper, IsShopItem);
	}
	else
	{
		for (FItemData& Item : InventoryItems)
		{
			if (Item.ItemClass == ItemSubclass)
			{
				if (Item.StackCount)
				{
					UseItem(ItemSubclass, ShopKeeper, IsShopItem);
				}
				return;
			}
		}
	}
}

void AInventoryShopTutCharacter::UseRemoveItem(TSubclassOf<AItem> ItemSubclass, bool UseItem, uint16 AmountToRemove)
{
	uint8 Index = 0;
	for (FItemData& Item : InventoryItems)
	{
		if (Item.ItemClass == ItemSubclass)
		{
			if (UseItem)
			{
				if (AItem* ItemCDO = ItemSubclass.GetDefaultObject())
				{
					if (ItemCDO->IsEquipable())
					{
						if (HasAuthority())
						{
							FActorSpawnParameters SpawnParams;
							SpawnParams.Owner = this;
							if (AItem* SpawnedItem = GetWorld()->SpawnActor<AItem>(ItemSubclass, SpawnParams))
							{
								SpawnedItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("spine_02"));
							}
						}
					}
					else
					{
						ItemCDO->Use(this, false);
					}
				}
			}
			Item.StackCount -= AmountToRemove;
			if (Item.StackCount <= 0)
			{
				InventoryItems.RemoveAt(Index);
			}
			break;
		}
		++Index;
	}
	if (IsLocallyControlled())
	{
		OnRep_InventoryItems();
	}
}

void AInventoryShopTutCharacter::Interact()
{
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 500.0f;
	if (HasAuthority())
	{
		Interact(Start, End);
	}
	else
	{
		Interact(Start, End);
		Server_Interact(Start, End);
	}
}

void AInventoryShopTutCharacter::Interact(FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		AShopKeeper* ShopKeeper = Cast<AShopKeeper>(HitResult.GetActor());
		if (ShopKeeper)
		{
			if (IsLocallyControlled())
			{
				ShopKeeper->Interact(this);
			}
			return;
		}
		
		if (IInteractableInterface* Interface = Cast<IInteractableInterface>(HitResult.GetActor()))
		{
			Interface->Interact(this);
		}
	}
}

bool AInventoryShopTutCharacter::Server_Interact_Validate(FVector Start, FVector End)
{
	return true;
}

void AInventoryShopTutCharacter::Server_Interact_Implementation(FVector Start, FVector End)
{
	Interact(Start, End);
}

void AInventoryShopTutCharacter::UpdateStats_Implementation(float NewHunger, float NewHealth)
{}

void AInventoryShopTutCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInventoryShopTutCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AInventoryShopTutCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AInventoryShopTutCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
