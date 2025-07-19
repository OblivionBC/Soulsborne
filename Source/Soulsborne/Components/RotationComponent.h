#pragma once

#include "RotationComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSBORNE_API URotationComponent : public UActorComponent
{
	GENERATED_BODY()
	URotationComponent();
	virtual void BeginPlay() override;

public:
	void StartSmoothTurnTo(FVector TargetLocation, float RotationSpeed, FSimpleDelegate OnFinished);
	void StartSmoothTurnTo(FVector InTargetLocation, float InRotationSpeed);

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AActor* Owner = nullptr;
	FVector TargetLocation;
	float RotationSpeed = 5.0f;
	FSimpleDelegate OnFinishedDelegate;
	bool bIsTurning = false;
};
