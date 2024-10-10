// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttackCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimMontage.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UCharacterAttackCombo::UCharacterAttackCombo()
{
	// Ability properties initialization
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage(TEXT("/Game/Soulsbourne/Animations/Gorka/Combat/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));
	if (Montage.Succeeded())
	{
		MontageToPlay = Montage.Object;
	}
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

		USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
		UAnimInstance* AnimInstance = nullptr;

		AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		if (AvatarActor)
		{
			SkeletalMeshComponent = AvatarActor->FindComponentByClass<USkeletalMeshComponent>();
			AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		}

		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ActorInfo->OwnerActor);
		if (PlayerCharacter)
		{
			// Bind to the player character's event dispatcher
			PlayerCharacter->OnMontageNotify.AddDynamic(this, &UCharacterAttackCombo::OnMontageNotifyStart);
		}

		if (SkeletalMeshComponent && AnimInstance)
		{
			// Perform your logic here
			AnimInstance->Montage_Play(MontageToPlay);
		}

		PerformComboAttack(Handle, ActorInfo, ASC);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

		ASC->RemoveLooseGameplayTags(ActivationOwnedTags);
		ASC->UnBlockAbilitiesWithTags(BlockAbilitiesWithTag);
	}
}

void UCharacterAttackCombo::OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) {
	UE_LOG(LogTemp, Log, TEXT("Binding OnPlayMontageNotifyBegin delegate"))
		UE_LOG(LogTemp, Log, TEXT("Notify Begin: %s"), *NotifyName.ToString());
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