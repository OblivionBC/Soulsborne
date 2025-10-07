#include "BTTask_ClearLockOnTarget.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_ClearLockOnTarget::UBTTask_ClearLockOnTarget(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Clear Lock on Target");
}

EBTNodeResult::Type UBTTask_ClearLockOnTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	URotationComponent* rotcomp = Cast<URotationComponent>(AIController->GetPawn()->GetComponentByClass(URotationComponent::StaticClass()));
	if (!rotcomp)
		return EBTNodeResult::Failed;

	rotcomp->StopLockOn();
	
	return EBTNodeResult::Succeeded;
}

