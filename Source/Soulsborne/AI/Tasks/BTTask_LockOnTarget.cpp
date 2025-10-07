#include "BTTask_LockOnTarget.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_LockOnTarget::UBTTask_LockOnTarget(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Lock on Target");
}

EBTNodeResult::Type UBTTask_LockOnTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedOwnerComp = &OwnerComp;
	AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	URotationComponent* rotcomp = Cast<URotationComponent>(AIController->GetPawn()->GetComponentByClass(URotationComponent::StaticClass()));
	if (!rotcomp)
		return EBTNodeResult::Failed;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
		return EBTNodeResult::Failed;

	TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor)
		return EBTNodeResult::Failed;

	rotcomp->LockOnTarget(TargetActor, 300.f);
	
	return EBTNodeResult::Succeeded;
}

