
#include "BT_Task_PlayBossIntro.h"

#include "AIController.h"
#include "../../Characters/BossCharacter.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soulsborne/AI/BossAIController.h"

UBTTask_PlayBossIntro::UBTTask_PlayBossIntro()
{
	NodeName = "Play Boss Intro Montage";
}

EBTNodeResult::Type UBTTask_PlayBossIntro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;
	BossCharacter = Cast<ABossCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	BossCharacter->GetCharacterMovement()->StopMovementImmediately();

	if (!BossCharacter || !BossCharacter->IntroMontage)
	{
		return EBTNodeResult::Failed;
	}

	if (UAnimInstance* AnimInstance = BossCharacter->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->Montage_Play(BossCharacter->IntroMontage) > 0.f)
		{
			MontageEndedDelegate.BindUObject(this, &UBTTask_PlayBossIntro::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, BossCharacter->IntroMontage);

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_PlayBossIntro::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (BossCharacter && BossCharacter->HUDComponent)
	{
		BossCharacter->HUDComponent->SetBossHudVisible(true);
		if (UBossPhaseComponent* cp = Cast<UBossPhaseComponent>(BossCharacter->GetComponentByClass(UBossPhaseComponent::StaticClass())))
		{
			cp->SetPhase(1);
		}

	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
