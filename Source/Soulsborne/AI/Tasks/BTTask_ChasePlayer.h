#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Soulsborne/Components/RotationComponent.h"
#include "BTTask_ChasePlayer.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_ChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChasePlayer(const FObjectInitializer& ObjectInitializer);

	/** Blackboard key: target actor to chase */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector DistanceKey;

	/** Minimum distance to stop chasing */
	UPROPERTY(EditAnywhere, Category = "Chase")
	float AcceptableDistance = 250.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
protected:
	/** Cached AI controller */
	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	URotationComponent* RotComp;

	UPROPERTY()
	APawn* AIPawn;

	UPROPERTY()
	AActor* TargetActor;
};
