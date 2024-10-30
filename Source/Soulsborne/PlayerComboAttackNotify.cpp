// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComboAttackNotify.h"
#include "CharacterAttackCombo.h"
#include "SoulsPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AttackCombo.h"


void UPlayerComboAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NEW NOTIFY CALLED"));

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (ASoulsPlayerCharacter* Character = Cast<ASoulsPlayerCharacter>(Owner))
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify IN ASC"));
				// Retrieve the active abilities and find the combo attack ability
				TArray<FGameplayAbilitySpec> ActiveAbilities = ASC->GetActivatableAbilities();
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
				//ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, ActiveAbilities);
				ASC->GetActivatableAbilities();

				for (FGameplayAbilitySpec Spec : ActiveAbilities)
				{
					UAttackCombo* ComboAbility = Cast<UAttackCombo>(Spec.Ability);
					if (ComboAbility)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FOUND IT"));
						ComboAbility->CheckContinueCombo(Character);
					}

				}
			}
		}
	}
}