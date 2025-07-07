// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseGameplayAbility.h"
#include "AbilitySystemComponent.h"	
#include "EnhancedInputComponent.h"


UBaseGameplayAbility::UBaseGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBaseGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	ACharacter* Owner = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (Owner) {
		SetInputBindings(ActorInfo, Spec);

		if (bActivateOnGranted)
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UBaseGameplayAbility::SetInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ACharacter* Owner = Cast<ACharacter>(ActorInfo->AvatarActor)) {
		if (const AController* PawnController = Owner->GetController())
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()))
			{
				if (UBaseGameplayAbility* AbilityInstance = Cast<UBaseGameplayAbility>(Spec.Ability.Get()))
				{
					if (AbilityInstance->AbilityInputAction) {
						//As long as the InputStartTriggerType was not set to None, we bind the event
						if (AbilityInstance->InputStartTriggerType != ETriggerEvent::None)
						{
							EnhancedInputComponent->BindAction(AbilityInstance->AbilityInputAction, AbilityInstance->InputStartTriggerType, AbilityInstance, &ThisClass::HandleInputPressedEvent, ActorInfo, Spec.Handle);

						}
						//As long as the InputStartTriggerType was not set to None, we bind the event
						if (AbilityInstance->InputCompletedTriggerType != ETriggerEvent::None)
						{
							EnhancedInputComponent->BindAction(AbilityInstance->AbilityInputAction, AbilityInstance->InputCompletedTriggerType, AbilityInstance, &ThisClass::HandleInputReleasedEvent, ActorInfo, Spec.Handle);

						}

					}
				}
			}
		}
	}
}

void UBaseGameplayAbility::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	//Implemented in child class
}

void UBaseGameplayAbility::HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	//Implemented in child class
}

void UBaseGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ACharacter* Owner = Cast<ACharacter>(ActorInfo->AvatarActor)) {
		if (const AController* PawnController = Owner->GetController())
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get()))
			{
				EnhancedInputComponent->ClearBindingsForObject(Spec.Ability.Get());
			}
		}
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}
