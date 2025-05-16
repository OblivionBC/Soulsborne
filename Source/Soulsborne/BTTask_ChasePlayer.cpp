// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_ChasePlayer.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get target location from blackboard via npc controller
	UE_LOG(LogTemp, Warning, TEXT("IN CHASE"))

		if (auto* const controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner())) {
			auto const PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

			//Move to the player location
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(controller, PlayerLocation);

			//Finish and return success
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}
	return EBTNodeResult::Failed;
}
