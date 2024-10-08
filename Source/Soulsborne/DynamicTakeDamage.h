// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SoulAttributeSet.h"
#include "DynamicTakeDamage.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API UDynamicTakeDamage : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UDynamicTakeDamage()
	{
		// Set the duration policy to instant
		DurationPolicy = EGameplayEffectDurationType::Instant;

		// Define the health modification
		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = USoulAttributeSet::GetHealthAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.ModifierMagnitude = FScalableFloat(-30.0f); // Example damage value

		Modifiers.Add(ModifierInfo);
	}
};
