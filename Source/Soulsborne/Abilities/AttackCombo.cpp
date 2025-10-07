// Class to execute a Combo Attack Implemented as a Gameplay Ability

#include "AttackCombo.h"
#include "../Characters/BaseCharacter.h"
#include "AbilitySystemComponent.h"	
#include "../GameplayTags/SoulsGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "Soulsborne/Characters/SoulsPlayerCharacter.h"

UAttackCombo::UAttackCombo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityInputAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Attack.IA_Attack")));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage(TEXT("/Game/Soulsbourne/Animations/Attacks/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));

	if (Montage.Succeeded())
	{
		DefaultMontage = Montage.Object;
	}

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.isAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.cantAct")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.isDead")));
}

void UAttackCombo::OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

void UAttackCombo::CheckContinueCombo(ACharacter* Character)
{
	bool bHasStamina = true;
	if (ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(Character))
	{
		bHasStamina = Player->UseStamina(10.0f);

	}
	if ((bContinueCombo || bHoldingAttack) && bHasStamina) {
		bContinueCombo = false;
	}
	else {
		if (Character) {
			ChooseMontage(Character);
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance()) {
				if (MontageToPlay) {
					AnimInstance->Montage_Stop(0.2f, MontageToPlay);
				}
			}
		}
	}
}

void UAttackCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AActor* Avatar = ActorInfo->AvatarActor.Get();
	ChooseMontage(Avatar);
	bHoldingAttack = true;
	bContinueCombo = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(Avatar))
	{
		if (!Player->UseStamina(5.0f)) EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		Player->StopStaminaRegen();
		Player->GetCharacterMovement()->bEnablePhysicsInteraction = false;

	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (ASC && AnimInstance)
		{
			AbilityMontageEnded.BindUObject(this, &UAttackCombo::OnAbilityMontageEnd);
			AnimInstance->Montage_Play(MontageToPlay);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, MontageToPlay);
		}
	}
}

void UAttackCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	bContinueCombo = false;
	bHoldingAttack = true;
	AbilityMontageEnded.Unbind();
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (Character && ASC && AnimInstance)
	{
		AnimInstance->OnMontageEnded.Clear();
		AbilityMontageEnded.Unbind();
	}
	if (UCharacterMovementComponent* Movement = Cast<UCharacterMovementComponent>(Character->GetComponentByClass(UCharacterMovementComponent::StaticClass())))
	{
		Movement->SetMovementMode(MOVE_Walking);
	};
	if (ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(Character))
	{
		Player->StartStaminaRegen();
	}
}

void UAttackCombo::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	if (ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (!Player->IsHoldingWeapon())
		{
			return;
		}
	}
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	Spec->InputPressed = true;

	//If the ability is currently active, the input is to continue the combo, else it is to start it
	if (Spec->IsActive())
	{
		ASC->AbilitySpecInputPressed(*Spec);
		bContinueCombo = true;
	}
	else
	{
		ASC->TryActivateAbility(SpecHandle);
		bHoldingAttack = true;
	}
}

void UAttackCombo::HandleInputReleasedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	bHoldingAttack = false;
}

void UAttackCombo::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
}

void UAttackCombo::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->OwnerActor)) {

		if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance()) {
			ChooseMontage(Character);
			if (MontageToPlay) {
				AnimInstance->Montage_Stop(0.2f, MontageToPlay);
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}

void UAttackCombo::ChooseMontage(AActor* Avatar)
{
	UE_LOG(LogTemp, Log, TEXT("In ChooseMontage: %s"), *Avatar->GetActorNameOrLabel());
	
	FGameplayTagContainer Tags;
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(Avatar))
	{
		if (Character->Ability1Mont)
		{
			MontageToPlay = Character->Ability1Mont;
		}
	}
	else
	{
		MontageToPlay = DefaultMontage;
	}
}
