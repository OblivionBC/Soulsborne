#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
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

	/** Minimum distance to stop chasing */
	UPROPERTY(EditAnywhere, Category = "Chase")
	float AcceptableDistance = 150.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	/** Cached AI controller */
	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	APawn* AIPawn;

	UPROPERTY()
	AActor* TargetActor;
};
