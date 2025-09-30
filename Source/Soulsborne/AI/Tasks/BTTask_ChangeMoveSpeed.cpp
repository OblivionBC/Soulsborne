#include "BTTask_ChangeMoveSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Components/RotationComponent.h"

UBTTask_ChangeMoveSpeed::UBTTask_ChangeMoveSpeed(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Change Move Speed");
}

EBTNodeResult::Type UBTTask_ChangeMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (!AIComp)
		return EBTNodeResult::Failed;

	APawn* character = AIComp->GetPawn();
	if (!character)
		return EBTNodeResult::Failed;
	

	if (UCharacterMovementComponent* move = character->FindComponentByClass<UCharacterMovementComponent>())
	{
		//move->SetMovementMode(Movement_Mode);
		move->MaxWalkSpeed = NewSpeed;
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
