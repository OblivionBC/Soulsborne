// Class to execute a Combo Attack Implemented as a Gameplay Ability

#include "AttackCombo.h"
#include "../Characters/BaseCharacter.h"
#include "../PlayerCombatComponent.h"
#include "AbilitySystemComponent.h"	
#include "../GameplayTags/SoulsGameplayTags.h"
#include "Soulsborne/Characters/BossCharacter.h"

UAttackCombo::UAttackCombo()
{
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
	ABaseCharacter* base = Cast<ABaseCharacter>(Character);
	if (bContinueCombo || bHoldingAttack) {
		bContinueCombo = false;
		UE_LOG(LogTemp, Display, TEXT("ContinueCombo In AttackCombo.cpp"));
		
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
	UE_LOG(LogTemp, Log, TEXT("In ChooseMontage: %s"), *Avatar->GetActorNameOrLabel());
	ChooseMontage(Avatar);
	
	UE_LOG(LogTemp, Display, TEXT("AttackCombo.cpp ACTIVATED"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
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
}

void UAttackCombo::HandleInputPressedEvent(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
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
	if (ACharacter* character = Cast<ACharacter>(ActorInfo->OwnerActor)) {

		if (UAnimInstance* AnimInstance = character->GetMesh()->GetAnimInstance()) {
			ChooseMontage(character);
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
	if (ABaseCharacter* character = Cast<ABaseCharacter>(Avatar))
	{
		UE_LOG(LogTemp, Display, TEXT("They are base"));
		UAbilitySystemComponent* ASC = character->GetAbilitySystemComponent();
		for (FGameplayTag tag : ASC->GetOwnedGameplayTags())
		{
			UE_LOG(LogTemp, Log, TEXT("Tag Name: %s"), *tag.GetTagName().ToString());
		}
		
		if (ASC->HasMatchingGameplayTag(FSoulsGameplayTags::Get().Class_Knight))
		{
			UE_LOG(LogTemp, Log, TEXT("Knight class detected."));
			MontageToPlay = DefaultMontage;
		}else
		{
			UE_LOG(LogTemp, Display, TEXT("Other class detected."));
			MontageToPlay = DefaultMontage;
		}
	}
	else
	{
		MontageToPlay = DefaultMontage;
	}
}
