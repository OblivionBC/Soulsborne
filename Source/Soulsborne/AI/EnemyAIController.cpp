// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../Characters/SoulsPlayerCharacter.h"
#include "../Characters/NonPlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
{
	SetupPerceptionSystem();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(InPawn))
	{
		if (UBehaviorTree* Tree = NPC->GetBehaviorTree())
		{
			UBlackboardComponent* BlackboardComp;
			UseBlackboard(Tree->BlackboardAsset, BlackboardComp);
			Blackboard = BlackboardComp;
			RunBehaviorTree(NPC->GetBehaviorTree());
		}
	}
}

void AEnemyAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig) {
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		SightConfig->SightRadius = 2400.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
		SightConfig->PeripheralVisionAngleDegrees = 360.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const Character = Cast<ASoulsPlayerCharacter>(Actor)) {
		GetBlackboardComponent()->SetValueAsBool("bSeeingTarget", Stimulus.WasSuccessfullySensed());
		GetBlackboardComponent()->SetValueAsObject("targetActor", Character);
		AttackTarget = Character;
	}
}
