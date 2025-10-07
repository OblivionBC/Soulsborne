#include "BTTask_TurnToPlayer.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_TurnToPlayer::UBTTask_TurnToPlayer(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Turn To Player");
}

EBTNodeResult::Type UBTTask_TurnToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;
	AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	Boss = Cast<ABossCharacter>(AIController->GetPawn());
	if (!Boss)
		return EBTNodeResult::Failed;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
		return EBTNodeResult::Failed;

	TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
		return EBTNodeResult::Failed;
	
	TurnFinished.BindUObject(this, &UBTTask_TurnToPlayer::OnTurnFinished);
	Boss->RotationComponent->StartSmoothTurnTo(TargetActor->GetActorLocation(), RotationSpeed, TurnFinished);
	return EBTNodeResult::InProgress;
}

void UBTTask_TurnToPlayer::OnTurnFinished()
{
	if (CachedOwnerComp)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
