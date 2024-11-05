// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "NonPlayerCharacter.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
{
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(InPawn))
	{
		if (UBehaviorTree* tree = NPC->GetBehaviorTree())
		{
			UBlackboardComponent* BlackboardComp;
			UseBlackboard(tree->BlackboardAsset, BlackboardComp);
			Blackboard = BlackboardComp;
			RunBehaviorTree(NPC->GetBehaviorTree());
		}
	}
}
