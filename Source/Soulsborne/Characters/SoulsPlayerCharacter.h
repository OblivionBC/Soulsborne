// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "BaseCharacter.h"
#include "../PlayerCombatComponent.h"
#include "../PlayerCombatInterface.h"
#include "../Abilities/SoulAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Soulsborne/Items/Item.h"
#include "Soulsborne/Items/PickupInterface.h"
#include "Soulsborne/Items/WeaponItem.h"
#include "Soulsborne/UI/PlayerHUDWidget.h"
#include "SoulsPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API ASoulsPlayerCharacter : public ABaseCharacter, public IPlayerCombatInterface, public IPickupInterface
{
	GENERATED_BODY()

	/**	----------------------------------- Functions ------------------------------------ **/
public:
	// Sets default values for this character's properties
	ASoulsPlayerCharacter();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerKilledSignature, AActor*, KilledPlayer);
	UPROPERTY(BlueprintAssignable)
	FOnPlayerKilledSignature OnPlayerKilled;
	
	

protected:
	/* Attribute Functions   */
	virtual void InitializeAttributes();
	virtual void Die() override;
	

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
	virtual void Drink(const FInputActionValue& Value);
	virtual void PlayerJump(const FInputActionValue& Value);
	virtual void LockCamera(const FInputActionValue& Value);

	UFUNCTION()
	virtual void Pickup_Implementation(TSubclassOf<UItem> Item);
	void CheckItemCategory(UItem* item, int slot);
	/**	----------------------------------- Properties ------------------------------------ **/
public:
	virtual void SoulsTakeDamage(float DamageAmount, FName DamageType) override;
	virtual void SoulsHeal(float HealAmount) override;
	void StartStaminaRegen();
	void StopStaminaRegen();
	void RegenStamina();
	bool UseStamina(const float StaminaAmnt);
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DrinkMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UItem*> InventorySlots;
	int itemsPicked = 0;
	int equippedItemIndex = 0;
	void SwapItem(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	FOnMontageEnded DrinkEnded;
	
protected:
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* CachedItemMesh;
		
	bool bCanRegenStamina = true;
	FTimerHandle StaminaRegenTimer;
	float StaminaRegenRate = 0.5f;
	float StaminaRegenInterval = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayEffect> RechargeStaminaEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DodgeAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayAbility> DrinkAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<UGameplayAbility> AttackComboAbility;
	/* UI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UPlayerHUDWidget* PlayerHUD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> HUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UProgressBar* HealthBar;
	
	void SetHealthProgressBar(float HealthProgress);
	/* Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* MyInputMappingContext;

	class UAIPerceptionStimuliSourceComponent* StimulusSource;
	void SetupStimulusSource();
	
	UFUNCTION()
	void EquipItem(TSubclassOf<UItem> Item);

	UFUNCTION()
	void EquipWeapon(TSubclassOf<UItem> Item);
};
