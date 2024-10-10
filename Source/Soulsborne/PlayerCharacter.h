// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
#include "SoulCharacter.h"

#include "PlayerCharacter.generated.h"


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDealDamage, AActor*, DamagedActor, float, Damage, AController*, InstigatedBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMontageNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);
UCLASS()
class SOULSBORNE_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IProgressBarInterface, public IPlayerCombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	UFUNCTION()
	void OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	void AttackComboNoDelegates(const FInputActionValue& Value);
	UPROPERTY()
	bool isComboActive;
	UFUNCTION()
	void OnComboMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UAnimMontage* MontageToPlay;
	/**	----------------------------------- Functions ------------------------------------ **/
	/* Attribute Functions   */
	virtual void InitializeAttributes();

	/* Progress Bar Interface Functions */
	virtual void GetHealth_Implementation(double& Result) const override;
	virtual void GetHealthAsRatio_Implementation(double& Result) const override;
	virtual void GetStamina_Implementation(double& Result) const override;
	virtual void GetMana_Implementation(double& Result) const override;
	virtual void GetManaAsRatio_Implementation(double& Result) const override;
	virtual void GetStaminaAsRatio_Implementation(double& Result) const override;

	/* Player Combat Interface Functions*/
	virtual void StartDamageTrace_Implementation() const override;
	virtual void EndDamageTrace_Implementation() const override;

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
	void ApplyDamage(ASoulCharacter* Target, float Damage);


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
	virtual void LockCamera(const FInputActionValue& Value);
	virtual void AttackCombo(const FInputActionValue& Value);

	/**	----------------------------------- Properties ------------------------------------ **/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UPlayerCombatComponent* PlayerCombatComponent;
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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	bool isAttacking;

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

	UPROPERTY()
	AActor* CameraLockActor;

	UPROPERTY()
	AActor* TargetLockIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	UClass* TargetLockIconClass;

	/* Equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> LHandArmamentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> RHandArmamentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* RHandArmament;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	AActor* LHandArmament;

	/* UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UUserWidget* PlayerHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidget;

	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* MyInputMappingContext;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FMontageNotifyDelegate OnMontageNotify;

};
