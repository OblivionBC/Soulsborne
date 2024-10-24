// Class to execute a Combo Attack Implemented as a Gameplay Ability

#include "AttackCombo.h"
#include "AbilitySystemComponent.h"	

UAttackCombo::UAttackCombo()
{
	AbilityInputAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Attack.IA_Attack")));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage(TEXT("/Game/Soulsbourne/Animations/Attacks/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));
	if (Montage.Succeeded())
	{
		if (GEngine) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Got Montage for New Attack COmbo"));

		}

		AbilityMontage = Montage.Object;
	}

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.isAirborne")));
}

void UAttackCombo::OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

void UAttackCombo::CheckContinueCombo(ACharacter * Character)
{
	if (bContinueCombo == true) {
		bContinueCombo = false;
	}
	else {

		if(Character) {
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance()) {
				if (AbilityMontage) {
					AnimInstance->Montage_Stop(0.2f, AbilityMontage);
				}
			}
		}
	}
}

void UAttackCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (ASC && AnimInstance)
		{
			AbilityMontageEnded.BindUObject(this, &UAttackCombo::OnAbilityMontageEnd);
			AnimInstance->Montage_Play(AbilityMontage);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, AbilityMontage);
		}

	}
}

void UAttackCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	bContinueCombo = false;
	AbilityMontageEnded.Unbind();
}

void UAttackCombo::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		Spec->InputPressed = true;

		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
			bContinueCombo = true;
		}
		else
		{
			// Ability is not active, so try to activate it
			ASC->TryActivateAbility(SpecHandle);
		}
	
	
}

void UAttackCombo::HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
}

void UAttackCombo::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
}

void UAttackCombo::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
