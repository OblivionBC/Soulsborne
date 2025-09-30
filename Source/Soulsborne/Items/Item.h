#pragma once

#include "Item.generated.h"
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class SOULSBORNE_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemCategory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMesh* ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int Number = 0;
};
