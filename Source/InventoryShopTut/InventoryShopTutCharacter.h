// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryShopTut/Public/Structs.h"
#include "InventoryShopTutCharacter.generated.h"

class AItem;
class AShopKeeper;

UCLASS(config=Game)
class AInventoryShopTutCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AInventoryShopTutCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	UFUNCTION(BlueprintCallable, Category = "TUTORIAL")
	void UseItem(TSubclassOf<AItem> ItemSubclass, AShopKeeper* ShopKeeper, bool IsShopItem = false);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UseItem(TSubclassOf<AItem> ItemSubclass, AShopKeeper* ShopKeeper, bool IsShopItem = false);

	void UseRemoveItem(TSubclassOf<AItem> ItemSubclass, bool UseItem, uint16 AmountToRemove = 1);
	
	void Interact();
	void Interact(FVector Start, FVector End);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact(FVector Start, FVector End);
	
	UFUNCTION(BlueprintNativeEvent, Category = "TUTORIAL")
	void UpdateStats(float NewHunger, float NewHealth);

	UPROPERTY(ReplicatedUsing = OnRep_InventoryItems, BlueprintReadWrite, Category = "TUTORIAL")
	TArray<FItemData> InventoryItems;
	UFUNCTION()
	void OnRep_InventoryItems();

	UFUNCTION(BlueprintImplementableEvent, Category = "TUTORIAL")
	void AddItemAndUpdateInventoryWidget(FItemData ItemData, const TArray<FItemData>& CurrentInventory = TArray<FItemData>());
	
	UPROPERTY(ReplicatedUsing = OnRep_Stats, BlueprintReadWrite, Category = "TUTORIAL")
	float Health;

	UPROPERTY(ReplicatedUsing = OnRep_Stats, BlueprintReadWrite, Category = "TUTORIAL")
	float Hunger;

	UFUNCTION()
	void OnRep_Stats();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentGold();

	UFUNCTION(BlueprintCallable)
	void RemoveGold(int32 AmountToRemove);

	UFUNCTION(BlueprintImplementableEvent)
	void OpenShop(AShopKeeper* OwningShop, const TArray<FItemData>& Items);

	void AddInventoryItem(FItemData ItemData);
	void AddHealth(float Value);
	void RemoveHunger(float Value);
};

