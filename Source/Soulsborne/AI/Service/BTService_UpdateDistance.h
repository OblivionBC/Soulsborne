#pragma once

#include "BehaviorTree/BTService.h"
#include "BTService_UpdateDistance.generated.h"

UCLASS()
class SOULSBORNE_API UBTService_UpdateDistance : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateDistance();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
