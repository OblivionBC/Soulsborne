// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "AbilitySystemInterface.h"
#include "SoulAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProgressBarInterface.h"
#include "PlayerCharacter.generated.h"




UCLASS()
class SOULSBORNE_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IProgressBarInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	/**	----------------------------------- Functions ------------------------------------ **/
	/* Attribute Functions   */
	virtual void InitializeAttributes();

	virtual void GetHealth_Implementation(double& Result) const override;
	virtual void GetHealthAsRatio_Implementation(double& Result) const override;
	virtual void GetStamina_Implementation(double& Result) const override;
	virtual void GetMana_Implementation(double& Result) const override;
	virtual void GetManaAsRatio_Implementation(double& Result) const override;
	virtual void GetStaminaAsRatio_Implementation(double& Result) const override;

	/* MISC */
	void AttatchEquipment(TSubclassOf<AActor> Equipment, FName socketName);

	/*  Base Functions  */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;



	/* Ability System Functions */
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GiveDefaultAbilities();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	virtual void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply);
	UFUNCTION(BlueprintCallable)
	 void HandleMeleeAttack();


	/* Input Functions */
	virtual void MoveForward(const FInputActionValue& Value);
	virtual void MoveRight(const FInputActionValue& Value);
	virtual void LookRight(const FInputActionValue& Value);
	virtual void LookUp(const FInputActionValue& Value);
	virtual void Attack(const FInputActionValue& Value);
	virtual void Roll(const FInputActionValue& Value);
	virtual void Block(const FInputActionValue& Value);
	virtual void BlockComplete(const FInputActionValue& Value);
	virtual void PlayerJump(const FInputActionValue& Value);

	/**	----------------------------------- Properties ------------------------------------ **/
	/*  Attributes */

	UPROPERTY()
	class USoulAttributeSet* Attributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStat = 99;

	/* Abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> StartingStatEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> RechargeStaminaEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> UseStamina;

	/* Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/* Equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> LHandArmament;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> RHandArmament;

	/* UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UUserWidget* PlayerHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidget;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* MyInputMappingContext;

};
