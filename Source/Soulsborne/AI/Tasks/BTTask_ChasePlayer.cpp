#include "BTTask_ChasePlayer.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Chase Player Until Close");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	AIPawn = AIController->GetPawn();
	if (!AIPawn)
		return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
		return EBTNodeResult::Failed;

	TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
		return EBTNodeResult::Failed;

	AIController->MoveToActor(TargetActor, AcceptableDistance);
	return EBTNodeResult::InProgress;
}

void UBTTask_ChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!AIController || !TargetActor || !AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	float Distance = FVector::Dist(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
	if (Distance <= AcceptableDistance)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}else
	{
		AIController->MoveToActor(TargetActor, AcceptableDistance);
	}
}

void UBTTask_ChasePlayer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}
