#include "DrinkPotion.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "../Characters/SoulsPlayerCharacter.h"

UDrinkPotion::UDrinkPotion()
{

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.isAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	DrinkEnded.BindUObject(this, &UDrinkPotion::DrinkEndedFunction);
}


void UDrinkPotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && ASC)
	{
		ABaseCharacter* Owner = Cast<ABaseCharacter>(ActorInfo->OwnerActor);
		Character = Owner;
		DrinkPotion();
	}
}

void UDrinkPotion::DrinkPotion()
{
	USkeletalMeshComponent* SkeletalMeshComponent = Character->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	
	if (DrinkPotionMontage)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = 600 * .2;
		CachedMesh = Character->EquippedItem->GetStaticMesh();
		Character->EquippedItem->SetStaticMesh(PotionMesh);
		AnimInstance->Montage_Play(DrinkPotionMontage);
		AnimInstance->Montage_SetEndDelegate(DrinkEnded, DrinkPotionMontage);
	}
}

void UDrinkPotion::DrinkEndedFunction(UAnimMontage* Montage, bool bInterrupted)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = 600;
	Character->EquippedItem->SetStaticMesh(CachedMesh);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDrinkPotion::OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

void UDrinkPotion::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->AvatarActor.IsValid())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (Character && ASC && AnimInstance)
		{
			AnimInstance->OnMontageEnded.Clear();
			DrinkEnded.Unbind();
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}