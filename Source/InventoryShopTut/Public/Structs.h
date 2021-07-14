#pragma once

#include "Structs.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UTexture2D* ItemImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ItemCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackCount = 1;
};