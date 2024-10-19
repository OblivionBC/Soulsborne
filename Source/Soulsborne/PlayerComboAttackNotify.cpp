// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComboAttackNotify.h"
#include "CharacterAttackCombo.h"
#include "PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "CharacterAttackCombo.h"


void UPlayerComboAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (APlayerCharacter* Character = Cast<APlayerCharacter>(Owner))
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify IN ASC"));
				// Retrieve the active abilities and find the combo attack ability
				TArray<FGameplayAbilitySpec*> ActiveAbilities;
				ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(), ActiveAbilities);

				for (FGameplayAbilitySpec* Spec : ActiveAbilities)
				{
					if (Spec->Ability->GetClass()->IsChildOf(UCharacterAttackCombo::StaticClass()) && Spec->IsActive())
					{
						UCharacterAttackCombo* ComboAbility = Cast<UCharacterAttackCombo>(Spec->Ability);
						if (ComboAbility)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FOUND IT"));
							ComboAbility->OnComboNotify();
						}

					}
				}
			}
		}
	}
}