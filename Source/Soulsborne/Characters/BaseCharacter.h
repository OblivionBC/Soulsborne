// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "../UI/ProgressBarInterface.h"
#include "../SoulAttributeSet.h"
#include "GameplayTagAssetInterface.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SOULSBORNE_API ABaseCharacter : public ACharacter, public IProgressBarInterface, public IGameplayTagAssetInterface
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
	virtual void SoulsTakeDamage(float DamageAmount, FName DamageType);
	virtual void SoulsHeal(float HealAmount);
	virtual void Die();
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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> StartingStatEffect;
public:
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ClassTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* RHandArmament;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* LHandArmament;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UArrowComponent* AttackArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* BaseAttackSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundBase* BaseAttackEffortSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsInvulnerable = false;

	UFUNCTION(BlueprintCallable)
	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimMontage * DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
	UAnimMontage * Ability1Mont;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DeathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* AttackedSound;
	
	UPROPERTY(EditAnywhere, Category = "VFX")
	UParticleSystem* DamagedFX;
	
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int NewMaxHealth);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStat = 200;
};
