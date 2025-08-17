#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Soulsborne/Components/RotationComponent.h"
#include "BTTask_ChangeMoveSpeed.generated.h"

UCLASS()
class SOULSBORNE_API UBTTask_ChangeMoveSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChangeMoveSpeed(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	TEnumAsByte<EMovementMode> Movement_Mode = MOVE_Custom;

	UPROPERTY(EditAnywhere, Category = "Speed")
	float NewSpeed = 100.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
