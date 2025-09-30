#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BTTaskNode.h"

#include "Soulsborne/Characters/BossCharacter.h"
#include "BTTask_BossLeap.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_BossLeap: public UBTTaskNode
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayAbilityEnded, UGameplayAbility* , bool /*bWasCancelled*/);
public:
	UBTTask_BossLeap();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAbilityEnded(UGameplayAbility* Ability);
	void NotAttacking();
	UPROPERTY(EditAnywhere)
	float EndWaitDelay;

private:
	UPROPERTY()
	ACharacter* CachedCharacter = nullptr;
	FTimerHandle TimerHandle;
	UPROPERTY()
	ABossCharacter* BossCharacter = nullptr;
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* CachedASC = nullptr;
};
