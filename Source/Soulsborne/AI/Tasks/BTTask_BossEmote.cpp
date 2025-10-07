#include "BTTask_BossEmote.h"

#include "AIController.h"
#include "BTTask_BossAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Soulsborne/Abilities/AttackCombo.h"
#include "Soulsborne/AI/BossAIController.h"

UBTTask_BossEmote::UBTTask_BossEmote()
{
	NodeName = "Looping Emote";
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_BossEmote::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	CachedCharacter = Cast<ACharacter>(AICon->GetPawn());
	if (!CachedCharacter || !EmoteMontage) return EBTNodeResult::Failed;

	PlayLoop(&OwnerComp);

	return EBTNodeResult::InProgress;
}

void UBTTask_BossEmote::PlayLoop(UBehaviorTreeComponent* OwnerComp)
{
	if (!CachedCharacter || !EmoteMontage) return;

	UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	float MontageDuration = 0.0f;
	if (EmoteMontage)
	{
		MontageDuration = AnimInstance->Montage_Play(EmoteMontage, 1.0f);
	}
	if (MontageDuration > 0.f)
	{
		// Set timer to restart montage when it ends
		CachedCharacter->GetWorldTimerManager().SetTimer(
			LoopTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_BossEmote::RestartMontageLoop, OwnerComp),
			MontageDuration - BlendOutTime,
			false);
	}
}

void UBTTask_BossEmote::RestartMontageLoop(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !CachedCharacter || !EmoteMontage) return;

	UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	// Replay the montage
	AnimInstance->Montage_Stop(BlendOutTime, EmoteMontage); // Smoothly blend out
	PlayLoop(OwnerComp); // Play again
}

void UBTTask_BossEmote::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (CachedCharacter && EmoteMontage)
	{
		UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && AnimInstance->Montage_IsPlaying(EmoteMontage))
		{
			AnimInstance->Montage_Stop(BlendOutTime, EmoteMontage);
		}

		CachedCharacter->GetWorldTimerManager().ClearTimer(LoopTimerHandle);
	}
}
