#include "BTTask_BossRockToss.h"
#include "AIController.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Abilities/BossRockThrow.h"
#include "Soulsborne/Animations/BossAnimInstance.h"
#include "Soulsborne/Characters/BossCharacter.h"

UBTTask_BossRockToss::UBTTask_BossRockToss()
{
	NodeName = "Perform Boss Rock Throw";
	bNotifyTaskFinished = true;
	ActiveAbilityTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("State.Attacking"));
}

EBTNodeResult::Type UBTTask_BossRockToss::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	CachedOwnerComp = &OwnerComp;
	if (!AICon) return EBTNodeResult::Failed;

	BossCharacter = Cast<ABossCharacter>(AICon->GetPawn());
	CachedASC = BossCharacter->GetAbilitySystemComponent();
	if (CachedASC)
	{
		
		for (const FGameplayAbilitySpec& Spec : CachedASC->GetActivatableAbilities())
		{
			if (Spec.Ability->GetClass()->IsChildOf(UBossRockThrow::StaticClass()))
			{
				UGameplayAbility* InstancedAbility = Spec.GetPrimaryInstance();
				if (InstancedAbility)
				{
					BossCharacter->GetCharacterMovement()->StopMovementImmediately();
					if (ABossAIController * ai = Cast<ABossAIController>(BossCharacter->GetController()))
					{
						ai->SetIsAttacking(true);
						ai->ClearFocus(EAIFocusPriority::Gameplay);
					}
					InstancedAbility->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_BossRockToss::OnAbilityEnded);
					CachedASC->TryActivateAbility(Spec.Handle);
					return EBTNodeResult::InProgress;
				}
				break;
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_BossRockToss::OnAbilityEnded(UGameplayAbility* Ability)
{
	Ability->OnGameplayAbilityEnded.RemoveAll(this);
	
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		if (ABossAIController * ai = Cast<ABossAIController>(BossCharacter->GetController()))
		{
			ai->SetIsAttacking(false);
		}
	}
}
