#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BossAIController.generated.h"


UCLASS()
class SOULSBORNE_API ABossAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABossAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	UFUNCTION()
	void SetupPerceptionSystem();
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	void SetDistanceToTarget(float Distance);
	void SetIsAttacking(bool isAttacking);
	// Convenience functions
	void SetTargetActor(AActor* Target);
	void SetIsEnraged(bool bIsEnraged);
	void SetPhase(int32 Phase);
	void UpdateHealthPercent(float Percent);
	void SetShouldUseAbility(bool bValue);

protected:
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComp;

	// Cached key IDs
	FBlackboard::FKey TargetActorKey;
	FBlackboard::FKey DistanceKey;
	FBlackboard::FKey IsEnragedKey;
	FBlackboard::FKey PhaseKey;
	FBlackboard::FKey HealthPercentKey;
	FBlackboard::FKey ShouldUseAbilityKey;
	FBlackboard::FKey IsAttackingKey;
};
