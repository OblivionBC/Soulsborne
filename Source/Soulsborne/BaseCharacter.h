// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "ProgressBarInterface.h"
#include "SoulAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SOULSBORNE_API ABaseCharacter : public ACharacter, public IProgressBarInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	//Returns the ASC for the Character
	UAbilitySystemComponent* GetAbilitySystemComponent();

	/* Getters for attributes set */
	virtual void GetHealth_Implementation(double& Result) const override;
	virtual void GetHealthAsRatio_Implementation(double& Result) const override;
	virtual void GetStamina_Implementation(double& Result) const override;
	virtual void GetMana_Implementation(double& Result) const override;
	virtual void GetManaAsRatio_Implementation(double& Result) const override;
	virtual void GetStaminaAsRatio_Implementation(double& Result) const override;

	/* MISC */
	void AttatchEquipment(TSubclassOf<AActor> Equipment, FName socketName);
	const FKey GetMovementDirection(UInputAction* Action);
	virtual void printAttributes();
	UFUNCTION(BlueprintImplementableEvent)
	void onDamaged();
protected:
	// Called when the game starts or when spawned
	UAbilitySystemComponent* AbilitySystemComponent;

	/* Equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> LHandArmamentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> RHandArmamentClass;

	/*  Attributes */
	UPROPERTY()
	class USoulAttributeSet* Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> StartingStatEffect;
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* RHandArmament;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* LHandArmament;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStat = 99;
};
