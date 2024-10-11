// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerEquipment.h"
#include "PlayerCombatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOULSBORNE_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FTimerHandle TimerHandle;
	void DamageTrace();
	void BeginDamageTrace(AActor* RHandArmament);
	void EndDamageTrace();
	void ApplyDamage(AActor* Target, float Damage);
	void TargetLockCamera();
	UPROPERTY()
	APlayerEquipment* Weapon;

	UPROPERTY()
	ACharacter* Owner;

	UPROPERTY()
	AActor* CameraLockActor;

	UPROPERTY()
	AActor* TargetLockIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	UClass* TargetLockIconClass;

	FTimerHandle TraceTimerHandle;

};
