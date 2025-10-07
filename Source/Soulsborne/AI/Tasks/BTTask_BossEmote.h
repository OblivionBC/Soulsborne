#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BTTaskNode.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "BTTask_BossEmote.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_BossEmote: public UBTTaskNode
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameplayAbilityEnded, UGameplayAbility* , bool /*bWasCancelled*/);
public:
	UBTTask_BossEmote();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Emote")
	UAnimMontage* EmoteMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Emote")
	float BlendOutTime = 0.25f;

private:
	UPROPERTY()
	ACharacter* CachedCharacter = nullptr;

	FTimerHandle LoopTimerHandle;

	void PlayLoop(UBehaviorTreeComponent* OwnerComp);
	void RestartMontageLoop(UBehaviorTreeComponent* OwnerComp);
};
