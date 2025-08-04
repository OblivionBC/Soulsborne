#include "BTTask_BossRockRip.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Soulsborne/Animations/BossAnimInstance.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "Soulsborne/Characters/Projectile.h"

UBTTask_BossRockRip::UBTTask_BossRockRip()
{
	NodeName = "Boss Rip Rock";
}

EBTNodeResult::Type UBTTask_BossRockRip::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	ABossCharacter* Boss = Cast<ABossCharacter>(AICon->GetPawn());
	if (!Boss) return EBTNodeResult::Failed;

	// Assuming RockProjectile is a TSubclassOf<AProjectile> exposed on the Boss class
	AProjectile* Rock = Boss->GetWorld()->SpawnActor<AProjectile>(Boss->RockProjectile, Boss->GetActorLocation()+100, FRotator::ZeroRotator);
	Rock->SetActorEnableCollision(false);
	if (!Rock) return EBTNodeResult::Failed;

	Rock->AttachToComponent(Boss->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);

	// Set animation state via AnimInstance
	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->BossPhase = EBossPhase::RockThrow;
		AnimInst->bIsHoldingRock = true;
	}

	return EBTNodeResult::Succeeded;
}
