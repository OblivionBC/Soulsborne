#pragma once

#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IBossCombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBossCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULSBORNE_API IBossCombatInterface
{
	GENERATED_BODY()

public:
	virtual void TraceForAttack(FGameplayTag AttackTag) = 0;
	virtual void PerformBasicAttack() = 0;
	virtual void PerformPhaseAbility(int32 Phase) = 0;
};
