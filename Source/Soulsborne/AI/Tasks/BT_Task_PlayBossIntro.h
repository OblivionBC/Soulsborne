#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_Task_PlayBossIntro.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_PlayBossIntro : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PlayBossIntro();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY()
	class ABossCharacter* BossCharacter;

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	FOnMontageEnded MontageEndedDelegate;
};
