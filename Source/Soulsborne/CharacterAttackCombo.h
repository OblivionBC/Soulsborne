// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "CharacterAttackCombo.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API UCharacterAttackCombo : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCharacterAttackCombo();
	UFUNCTION()
	void OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	void PerformComboAttack(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, UAbilitySystemComponent* ASC);

	void InitializeAbilityTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, UAbilitySystemComponent* ASC);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MontageToPlay;
};
