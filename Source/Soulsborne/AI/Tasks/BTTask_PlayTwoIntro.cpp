
#include "BTTask_PlayTwoIntro.h"

#include "AIController.h"
#include "BTTask_PlayBossIntro.h"
#include "../../Characters/BossCharacter.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/LowLevelTestAdapter.h"
#include "Soulsborne/AI/BossAIController.h"

UBTTask_PlayTwoIntro::UBTTask_PlayTwoIntro()
{
	NodeName = "Play Phase Two Intro";
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_PlayTwoIntro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Log, TEXT("Task Played"));

  	CachedOwnerComp = &OwnerComp;
	BossCharacter = Cast<ABossCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	BossCharacter->bIsInvulnerable = true;
	BossCharacter->GetCharacterMovement()->StopMovementImmediately();

	if (!BossCharacter || !BossCharacter->EnrageMontage)
	{
		return EBTNodeResult::Failed;
	}

	if (UAnimInstance* AnimInstance = BossCharacter->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->Montage_Play(BossCharacter->EnrageMontage) > 0.f)
		{
			MontageEndedDelegate.BindUObject(this, &UBTTask_PlayTwoIntro::OnMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, BossCharacter->EnrageMontage);
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_PlayTwoIntro::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (BossCharacter && BossCharacter->HUDComponent)
	{
		BossCharacter->HUDComponent->SetBossHudVisible(true);
		if (UBossPhaseComponent* CP = Cast<UBossPhaseComponent>(BossCharacter->GetComponentByClass(UBossPhaseComponent::StaticClass())))
		{
			CP->SetPhase(2);
			if (ABossAIController* Controller = Cast<ABossAIController>(BossCharacter->GetController()))
			{
				Controller->SetIsEnraged(true);
				BossCharacter->bIsInvulnerable = false;
				Controller->SetbIsCombatEngaged(true);
			}
		}

	}

	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}
