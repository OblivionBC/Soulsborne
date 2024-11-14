// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPlayerLocation.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {

	UE_LOG(LogTemp, Warning, TEXT("IN PLAYER LOCATION FIUND"))

		if (auto* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			//Get the Player Location
			auto const PlayerLocation = Player->GetActorLocation();
			if (SearchRandom) {
				FNavLocation Loc;

				//Get the nav system and generate a random location
				if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld())) {
					//try to get a random location near the player
					if (NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc)) {
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);

						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						return EBTNodeResult::Succeeded;
					}
				}
			}
			else {
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}


	return EBTNodeResult::Failed;
}
