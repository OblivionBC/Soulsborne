// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameplayAbility.h"
#include "../Characters/BaseCharacter.h"
#include "DrinkPotion.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API UDrinkPotion : public UBaseGameplayAbility
{
	GENERATED_BODY()
	UDrinkPotion();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DrinkPotionMontage;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void DrinkPotion();
	void DrinkEndedFunction(UAnimMontage* Montage, bool bInterrupted);
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	FOnMontageEnded DrinkEnded;
	UPROPERTY()
	UStaticMesh* CachedMesh;
	UPROPERTY()
	UStaticMesh* PotionMesh;
	UPROPERTY()
	ABaseCharacter* Character;
};
