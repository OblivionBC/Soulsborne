// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttackCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UCharacterAttackCombo::UCharacterAttackCombo()
{
	// Ability properties initialization
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SwordCombo(TEXT("/Game/Soulsbourne/Animations/Attacks/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));
	if (SwordCombo.Succeeded())
	{

		MontageToPlay = SwordCombo.Object;
	}
	bContinueCombo = false;
	bIsComboActive = false;
}

void UCharacterAttackCombo::InitializeAbilityTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, UAbilitySystemComponent* ASC) {
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));

	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Jump")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Attack")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));

	ASC->AddLooseGameplayTags(ActivationOwnedTags);
	if (ASC->AreAbilityTagsBlocked(ActivationBlockedTags))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	ASC->BlockAbilitiesWithTags(BlockAbilitiesWithTag);
	ASC->CancelAbilities(&CancelAbilitiesWithTag, nullptr, nullptr);
}

void UCharacterAttackCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && ASC)
	{
		InitializeAbilityTags(Handle, ActorInfo, ActivationInfo, ASC);

		//Make it so that the 
		PerformComboAttack(Handle, ActorInfo, ASC);
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UCharacterAttackCombo::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MontageToPlay)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
	}
}

void UCharacterAttackCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MontageToPlay && ActorInfo->AvatarActor.IsValid())
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (Character && ASC)
		{
			Character->StopAnimMontage(MontageToPlay);
			bIsComboActive = false;
			bContinueCombo = false;
			ASC->UnBlockAbilitiesWithTags(BlockAbilitiesWithTag);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCharacterAttackCombo::OnComboNotify() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify"));
	if (bContinueCombo)
	{
		bContinueCombo = false;
	}
	else
	{
		// Stop combo logic
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
void UCharacterAttackCombo::PerformComboAttack(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, UAbilitySystemComponent* ASC)
{
	// Logic for performing the combo attack
	// This could involve playing animations, applying damage, etc.
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	UAnimInstance* AnimInstance = nullptr;

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (AvatarActor)
	{

	}

	if (SkeletalMeshComponent && AnimInstance)
	{
		// Perform your logic here
		//AnimInstance->Montage_Play(MontageToPlay);

	}
}