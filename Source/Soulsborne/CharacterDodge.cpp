// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterDodge.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttackCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimMontage.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MovementDirectionENum.h"

UCharacterDodge::UCharacterDodge()
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





}

void UCharacterDodge::InitializeAbilityTags(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, UAbilitySystemComponent* ASC) {
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));

	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));
	//Character.AttackCombo
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Jump")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Block")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.isAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll")));
	if (ASC->AreAbilityTagsBlocked(ActivationBlockedTags))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	ASC->AddLooseGameplayTags(ActivationOwnedTags);
	ASC->BlockAbilitiesWithTags(BlockAbilitiesWithTag);
	ASC->CancelAbilities(&CancelAbilitiesWithTag, nullptr, nullptr);
}

void UCharacterDodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo) && ASC)
	{
		InitializeAbilityTags(Handle, ActorInfo, ActivationInfo, ASC);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Roll Called"));
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(ActorInfo->OwnerActor);
		if (PlayerCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Roll!"));
			Dodge(PlayerCharacter);
		}
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UCharacterDodge::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DodgeR || Montage == DodgeF || Montage == DodgeL || Montage == DodgeB)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
	}
}

void UCharacterDodge::Dodge(APlayerCharacter* Player)
{
	USkeletalMeshComponent* SkeletalMeshComponent = Player->FindComponentByClass<USkeletalMeshComponent>();
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (SkeletalMeshComponent && AnimInstance)
	{
		MontDelegate.BindUObject(this, &UCharacterDodge::OnMontageEnded);

		if (Player->GetCharacterMovement()->bUseControllerDesiredRotation == false) {
			UE_LOG(LogTemp, Warning, TEXT("UsingRotation"));
			AnimInstance->Montage_Play(DodgeF);
			AnimInstance->Montage_SetEndDelegate(MontDelegate, DodgeF);
			return;
		}

		FKey KeyDown = Player->DirectionKey;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, KeyDown.GetFName().ToString());
		if (KeyDown.GetFName().ToString() == "A") {
			UE_LOG(LogTemp, Warning, TEXT("A Clicked"));
			AnimInstance->Montage_Play(DodgeL);
			AnimInstance->Montage_SetEndDelegate(MontDelegate, DodgeL);
			UE_LOG(LogTemp, Warning, TEXT("Montage played and delegte set"));
			return;
		}
		else if (KeyDown.GetFName().ToString() == "S") {
			UE_LOG(LogTemp, Warning, TEXT("S Clicked"));
			AnimInstance->Montage_Play(DodgeB);
			AnimInstance->Montage_SetEndDelegate(MontDelegate, DodgeB);
			UE_LOG(LogTemp, Warning, TEXT("Montage played and delegte set"));
			return;
		}
		else if (KeyDown.GetFName().ToString() == "D") {
			UE_LOG(LogTemp, Warning, TEXT("D Clicked"));
			AnimInstance->Montage_Play(DodgeR);
			AnimInstance->Montage_SetEndDelegate(MontDelegate, DodgeR);
			UE_LOG(LogTemp, Warning, TEXT("Montage played and delegte set"));
			return;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("W Clicked"));
			AnimInstance->Montage_Play(DodgeF);
			AnimInstance->Montage_SetEndDelegate(MontDelegate, DodgeF);
			return;
		}

	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UCharacterDodge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->AvatarActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability end has valid actor and dodge_r"));
		ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (Character && ASC && AnimInstance)
		{
			//Character->StopAnimMontage(DodgeR);
			ASC->UnBlockAbilitiesWithTags(BlockAbilitiesWithTag);
			AnimInstance->OnMontageEnded.Clear();
			UE_LOG(LogTemp, Warning, TEXT("Ability Ended"));
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}