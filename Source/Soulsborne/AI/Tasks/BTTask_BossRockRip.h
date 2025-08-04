#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Soulsborne/Animations/BossAnimInstance.h"
#include "BTTask_BossRockRip.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_BossRockRip : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_BossRockRip();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	EBossPhase OldPhase;
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName AttachSocketName = "RightHandSocket";
};
