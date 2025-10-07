#include "BTTask_BossAttack.h"

#include "../../Characters/BossCharacter.h"
#include "AIController.h"
#include "GameplayTagsManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Soulsborne/Abilities/AttackCombo.h"
#include "Soulsborne/Abilities/BossAttack.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagsManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Components/IBossCombatInterface.h"

UBTTask_PerformAttack::UBTTask_PerformAttack()
{
	NodeName = "Perform Boss Attack";
	bNotifyTaskFinished = true;
	ActiveAbilityTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("State.Attacking"));
}

EBTNodeResult::Type UBTTask_PerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
			if (Spec.Ability->GetClass()->IsChildOf(UBossAttack::StaticClass()))
			{
				UGameplayAbility* InstancedAbility = Spec.GetPrimaryInstance();
				if (InstancedAbility)
				{
					BossCharacter->GetCharacterMovement()->StopMovementImmediately();
					if (ABossAIController* AI = Cast<ABossAIController>(BossCharacter->GetController()))
					{
						AI->SetIsAttacking(true);
					}
					CachedASC->TryActivateAbility(Spec.Handle);
					InstancedAbility->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_PerformAttack::OnAbilityEnded);
					return EBTNodeResult::InProgress;
				}
				break;
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_PerformAttack::OnAbilityEnded(UGameplayAbility* Ability)
{
	Ability->OnGameplayAbilityEnded.RemoveAll(this);
	
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		if (ABossAIController* AI = Cast<ABossAIController>(BossCharacter->GetController()))
		{
			AI->SetIsAttacking(false);
		}
	}
}