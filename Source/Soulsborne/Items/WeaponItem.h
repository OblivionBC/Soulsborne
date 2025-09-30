#pragma once

#include "Item.h"
#include "WeaponItem.generated.h"

UCLASS(Blueprintable)
class SOULSBORNE_API UWeaponItem : public UItem
{
	GENERATED_BODY()

	UWeaponItem();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float AttackPower;

};
