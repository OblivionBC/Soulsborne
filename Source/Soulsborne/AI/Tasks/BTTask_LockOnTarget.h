#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "BTTask_LockOnTarget.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_LockOnTarget : public UBTTaskNode
{
	GENERATED_BODY()
	DECLARE_DELEGATE(FTurnFinished)
public:
	UBTTask_LockOnTarget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category = "Task Settings")
	float RotationSpeed = 50.0f;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void OnTurnFinished();
protected:
	UPROPERTY()
	AAIController* AIController;
	FTurnFinished TurnFinished;
	
	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;
	UPROPERTY()
	ABossCharacter* Boss;

	UPROPERTY()
	AActor* TargetActor;
};
