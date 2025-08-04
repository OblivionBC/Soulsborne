#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Soulsborne/ENUM/BossPhaseENum.h"
#include "BossAnimInstance.generated.h"

UCLASS()
class SOULSBORNE_API UBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boss")
	EBossPhase BossPhase = EBossPhase::Phase1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Boss")
	bool bIsHoldingRock = false;
};
