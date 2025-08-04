#pragma once

#include "CoreMinimal.h"
#include "BossPhaseENum.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1     UMETA(DisplayName = "Phase 1"),
	Phase2     UMETA(DisplayName = "Phase 2"),
	RockThrow  UMETA(DisplayName = "Rock Throwing")
};
