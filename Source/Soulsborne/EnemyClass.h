// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoulCharacter.h"
#include "PlayerCharacter.h"
#include "EnemyClass.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API AEnemyClass : public ASoulCharacter
{
	GENERATED_BODY()
	//Ability System Component 
	//This is a bridge between the Charactor and the Gameplay Ability System
	//Gameplay ability system is a framwework for building attributes, abilities, and interactions that an actor can own and trigger

public:
	// Sets default values for this character's properties
	AEnemyClass();

	UFUNCTION()
	void Grow(AActor* DamagedActor, float Damage, AController* InstigatedBy);
	virtual void Damaged() override;
	void printAttributes();
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* HitMontage;
protected:
	UPROPERTY(EditAnywhere)
	APlayerCharacter* PlayerCharacter;
};