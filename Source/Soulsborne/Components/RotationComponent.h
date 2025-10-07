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
	void StartSmoothTurnTo(AActor* NewTargetActor, float InRotationSpeed, FSimpleDelegate OnFinished);
	void StopLockOn();
	void LockOnTarget(AActor* NewTargetActor, float InRotationSpeed);

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void HandleTurnTimeout();

private:
	FTimerHandle TurnTimeoutHandle;
	float TurnTimeoutDuration = 5.0f;
	UPROPERTY()
	ACharacter* Owner = nullptr;
	FVector TargetLocation;
	UPROPERTY()
	AActor* TargetActor = nullptr;
	float RotationSpeed = 5.0f;
	FSimpleDelegate OnFinishedDelegate;
	bool bIsTurning = false;
	bool bIsLocked = false;
};
