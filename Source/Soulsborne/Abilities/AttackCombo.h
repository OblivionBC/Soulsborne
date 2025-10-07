// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "AttackCombo.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API UAttackCombo : public UBaseGameplayAbility
{
	GENERATED_BODY()
	UAttackCombo();
public:
	UFUNCTION()
	void OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted);
	void CheckContinueCombo(ACharacter* Character);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* DefaultMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* BossMontage;
	
	FOnMontageEnded AbilityMontageEnded;

	UPROPERTY(BlueprintReadWrite)
	bool bContinueCombo = false;
	bool bHoldingAttack = true;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle) override;
	virtual void HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle) override;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	void ChooseMontage(AActor* Avatar);
private:
	UAnimMontage* MontageToPlay;
};
