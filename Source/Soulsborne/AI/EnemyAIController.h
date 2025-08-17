// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISense.h"
#include "Soulsborne/Characters/SoulsPlayerCharacter.h"
#include "EnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnPossess(APawn* InPawn) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ASoulsPlayerCharacter* AttackTarget;
private:
	class UAISenseConfig_Sight* SightConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
