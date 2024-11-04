// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "AbilitySystemInterface.h"
#include "PlayerCombatComponent.h"
#include "ProgressBarInterface.h"
#include "PlayerCombatInterface.h"
#include "SoulAttributeSet.h"
#include "SoulsPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API ASoulsPlayerCharacter : public ABaseCharacter, public IPlayerCombatInterface
{
	GENERATED_BODY()

	/**	----------------------------------- Functions ------------------------------------ **/
public:
	// Sets default values for this character's properties
	ASoulsPlayerCharacter();

protected:
	/* Attribute Functions   */
	virtual void InitializeAttributes();
	virtual void SoulsTakeDamage(float DamageAmount, FName DamageType) override;

	/* Player Combat Interface Functions*/
	virtual void StartDamageTrace_Implementation() const override;
	virtual void EndDamageTrace_Implementation() const override;

	/*  Base Functions  */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Ability System Functions */
	virtual void GiveDefaultAbilities();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	virtual void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply);
	
	/* Input Functions */
	virtual void MoveForward(const FInputActionValue& Value, UInputAction* Action);
	virtual void MoveRight(const FInputActionValue& Value, UInputAction* Action);
	virtual void LookRight(const FInputActionValue& Value);
	virtual void LookUp(const FInputActionValue& Value);
	virtual void Block(const FInputActionValue& Value);
	virtual void BlockComplete(const FInputActionValue& Value);
	virtual void PlayerJump(const FInputActionValue& Value);
	virtual void LockCamera(const FInputActionValue& Value);

	/**	----------------------------------- Properties ------------------------------------ **/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UPlayerCombatComponent* PlayerCombatComponent;

	//This is used to determine which direction the player is moving towards for rolling / directional abilities
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FKey DirectionKey;

		/* Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HitMontage;
protected:
	/* Abilities */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> RechargeStaminaEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> UseStamina;

	/* UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UUserWidget* PlayerHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidget;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* MyInputMappingContext;


};
