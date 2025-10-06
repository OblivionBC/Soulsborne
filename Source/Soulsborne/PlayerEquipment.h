// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "PlayerEquipment.generated.h"

UCLASS()
class SOULSBORNE_API APlayerEquipment : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerEquipment();
	UArrowComponent* GetStartAttackTrace();
	UArrowComponent* GetEndAttackTrace();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DamageType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "root")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* StartAttackTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrows")
	UArrowComponent* EndAttackTrace;
};
