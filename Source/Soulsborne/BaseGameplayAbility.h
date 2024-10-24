// Base ability class to be used as the parent for any gameplay abilities made

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SOULSBORNE_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	public:
	UBaseGameplayAbility();
	
	//Implemented in the AvatarSet to activate ability on granted if this is true
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Ability")
	bool bActivateOnGranted = false;

	//Used to bind the action to the players enhanced input component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Ability")
	UInputAction* AbilityInputAction = nullptr;

	//Trigger types for the ability
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	ETriggerEvent InputStartTriggerType = ETriggerEvent::Started;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	ETriggerEvent InputCompletedTriggerType = ETriggerEvent::Completed;

	protected:
		//Logic for when added to the avatar
		virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

		//Binds the Input Action to the Enhanced Input Component of the ACtor
		void SetInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

		//To Be Implemented in the child
		virtual void HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);
		virtual void HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);

		//Unbinds from the Enhanced INput component
		virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;


};
