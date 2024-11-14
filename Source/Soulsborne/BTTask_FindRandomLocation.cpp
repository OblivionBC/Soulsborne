// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocation.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "NavigationSystem.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get AI Controller
	UE_LOG(LogTemp, Warning, TEXT("IN RANDOM LOCATION FIUND"))
		if (auto* const controller = Cast<AEnemyAIController>(OwnerComp.GetAIOwner())) {
			if (auto* const npc = controller->GetPawn()) {
				//Get NPC Location
				auto const Origin = npc->GetActorLocation();

				//Get Nav system and generate random location
				if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld())) {
					FNavLocation Location;
					if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Location)) {
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Location.Location);
					}

					//Finish the task as succeeded
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	return EBTNodeResult::Failed;
}
