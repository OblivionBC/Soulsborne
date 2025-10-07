#pragma once

#include "CoreMinimal.h"
#include "DamageTypeENum.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Blunt     UMETA(DisplayName = "Blunt"),
	Slash     UMETA(DisplayName = "Slash"),
	Pierce  UMETA(DisplayName = "Pierce"),
	Magic  UMETA(DisplayName = "Magic"),
	Projectile  UMETA(DisplayName = "Projectile")
};
