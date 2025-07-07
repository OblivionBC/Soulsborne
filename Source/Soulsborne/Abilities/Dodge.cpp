#include "Dodge.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "../Characters/SoulsPlayerCharacter.h"
#include "../MovementDirectionENum.h"

UDodge::UDodge()
{
	// Ability properties initialization

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CDodgeF(TEXT("/Game/Soulsbourne/Animations/Dodge/MONT_Dodge_F.MONT_Dodge_F"));
	if (CDodgeF.Succeeded())
	{
		DodgeF = CDodgeF.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CDodgeB(TEXT("/Game/Soulsbourne/Animations/Dodge/MONT_Dodge_B.MONT_Dodge_B"));
	if (CDodgeB.Succeeded())
	{
		DodgeB = CDodgeB.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CDodgeL(TEXT("/Game/Soulsbourne/Animations/Dodge/MONT_Dodge_L.MONT_Dodge_L"));
	if (CDodgeL.Succeeded())
	{
		DodgeL = CDodgeL.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> CDodgeR(TEXT("/Game/Soulsbourne/Animations/Dodge/MONT_Dodge_R.MONT_Dodge_R"));
	if (CDodgeR.Succeeded())
	{
		DodgeR = CDodgeR.Object;
	}
	AbilityInputAction = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Roll.IA_Roll")));
	InputCompletedTriggerType = ETriggerEvent::None;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.isAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
}


void UDodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && ASC)
	{
		ABaseCharacter* Owner = Cast<ABaseCharacter>(ActorInfo->OwnerActor);
		if (Owner)
		{
			Dodge(Owner);
		}
	}
}

void UDodge::OnAbilityMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

void UDodge::Dodge(ABaseCharacter* Player)
{
	USkeletalMeshComponent* SkeletalMeshComponent = Player->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	ASoulsPlayerCharacter* SoulsPlayerCharacter = Cast<ASoulsPlayerCharacter>(Player);
	if (SkeletalMeshComponent && AnimInstance)
	{
		AbilityMontageEnded.BindUObject(this, &UDodge::OnAbilityMontageEnd);

		//Not Locked on a Target
		if (Player->GetCharacterMovement()->bUseControllerDesiredRotation == false) {
			AnimInstance->Montage_Play(DodgeF);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, DodgeF);
			return;
		}

		FKey KeyDown = SoulsPlayerCharacter->DirectionKey;
		//Player is holding down A (Roll Left)
		if (KeyDown.GetFName().ToString() == "A") {
			AnimInstance->Montage_Play(DodgeL);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, DodgeL);
			return;
		}
		//Player is holding down S (Roll Backwards)
		else if (KeyDown.GetFName().ToString() == "S") {
			AnimInstance->Montage_Play(DodgeB);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, DodgeB);
			return;
		}
		//Player is holding down D (Roll Right)
		else if (KeyDown.GetFName().ToString() == "D") {
			AnimInstance->Montage_Play(DodgeR);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, DodgeR);
			return;
		}
		//Player is holding down W (Roll Forward)
		else {
			AnimInstance->Montage_Play(DodgeF);
			AnimInstance->Montage_SetEndDelegate(AbilityMontageEnded, DodgeF);
			return;
		}

	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->AvatarActor.IsValid())
	{
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (Character && ASC && AnimInstance)
		{
			AnimInstance->OnMontageEnded.Clear();
			AbilityMontageEnded.Unbind();
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDodge::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	Spec->InputPressed = true;

	if (!Spec->IsActive())
	{
		ASC->TryActivateAbility(SpecHandle);
	}

}

void UDodge::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
}

void UDodge::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}