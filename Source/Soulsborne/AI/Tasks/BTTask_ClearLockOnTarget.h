#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "BTTask_ClearLockOnTarget.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_ClearLockOnTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ClearLockOnTarget(const FObjectInitializer& ObjectInitializer);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
