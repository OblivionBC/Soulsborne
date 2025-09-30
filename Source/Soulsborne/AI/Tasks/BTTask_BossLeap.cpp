#include "BTTask_BossLeap.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soulsborne/Abilities/AttackCombo.h"
#include "Soulsborne/Abilities/BossLeap.h"
#include "Soulsborne/AI/BossAIController.h"

UBTTask_BossLeap::UBTTask_BossLeap()
{
	NodeName = "Leap Attack";
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_BossLeap::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	BossCharacter = Cast<ABossCharacter>(AICon->GetPawn());
	CachedOwnerComp = &OwnerComp;
	if (!AICon) return EBTNodeResult::Failed;

	BossCharacter = Cast<ABossCharacter>(AICon->GetPawn());
	CachedASC = BossCharacter->GetAbilitySystemComponent();
	if (CachedASC)
	{
		for (const FGameplayAbilitySpec& Spec : CachedASC->GetActivatableAbilities())
		{
			if (Spec.Ability->GetClass()->IsChildOf(UBossLeap::StaticClass()))
			{
				UGameplayAbility* InstancedAbility = Spec.GetPrimaryInstance();
				if (InstancedAbility)
				{
					BossCharacter->GetCharacterMovement()->StopMovementImmediately();
					if (ABossAIController * ai = Cast<ABossAIController>(BossCharacter->GetController()))
					{
						ai->SetIsAttacking(true);
					}
					CachedASC->TryActivateAbility(Spec.Handle);
					InstancedAbility->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_BossLeap::OnAbilityEnded);
					return EBTNodeResult::InProgress;
				}
				break;
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_BossLeap::OnAbilityEnded(UGameplayAbility* Ability)
{
	Ability->OnGameplayAbilityEnded.RemoveAll(this);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UBTTask_BossLeap::NotAttacking,
		1.5f,   // float seconds to wait
		false   // don't loop
	);
}

void UBTTask_BossLeap::NotAttacking()
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		if (ABossAIController * ai = Cast<ABossAIController>(BossCharacter->GetController()))
		{
			ai->SetIsAttacking(false);
		}
	}
}
