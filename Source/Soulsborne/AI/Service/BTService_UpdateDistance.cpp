#include "BTService_UpdateDistance.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateDistance::UBTService_UpdateDistance()
{
	NodeName = "Update Distance to Target";
	Interval = 0.2f;
	bNotifyBecomeRelevant = true;
}

void UBTService_UpdateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* BB = OwnerComp.GetBlackboardComponent();
	auto* AI = OwnerComp.GetAIOwner();
	if (!BB || !AI) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target)
	{
		APawn * Boss = AI->GetPawn();
		float RawDistance = FVector::Dist(Boss->GetActorLocation(), Target->GetActorLocation());

		float AdjustedDistance = RawDistance 
			- Boss->GetSimpleCollisionRadius()
			- Target->GetSimpleCollisionRadius();
		BB->SetValueAsFloat(DistanceKey.SelectedKeyName, AdjustedDistance);
	}
}
