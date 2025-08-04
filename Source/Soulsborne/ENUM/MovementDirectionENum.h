#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MovementDirectionENum.generated.h"

UENUM(BlueprintType)
enum class EMovementDirectionENum : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};