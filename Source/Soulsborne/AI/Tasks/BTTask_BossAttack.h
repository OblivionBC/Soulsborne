#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BTTaskNode.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "BTTask_BossAttack.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_PerformAttack : public UBTTaskNode
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayAbilityEnded, UGameplayAbility* , bool /*bWasCancelled*/);
public:
	UBTTask_PerformAttack();
	FGameplayTag ActiveAbilityTag;
	UPROPERTY()
	UAbilityTask_WaitGameplayTagRemoved* WaitTagRemovedTask;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnAbilityEnded(UGameplayAbility* Ability);
	
	FDelegateHandle AbilityEndHandle;
	
private:
	UPROPERTY()
	UAbilitySystemComponent* CachedASC;
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
	UPROPERTY()
	ABossCharacter* BossCharacter;
};
