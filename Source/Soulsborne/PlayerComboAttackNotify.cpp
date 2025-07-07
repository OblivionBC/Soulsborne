// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComboAttackNotify.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/AttackCombo.h"
#include "Characters/SoulsPlayerCharacter.h"


void UPlayerComboAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	UE_LOG(LogTemp, Warning, TEXT("ComboAttackNotfiy Is Used"));

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (ASoulsPlayerCharacter* Character = Cast<ASoulsPlayerCharacter>(Owner))
		{
			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC)
			{
				// Retrieve the active abilities and find the combo attack ability
				TArray<FGameplayAbilitySpec> ActiveAbilities = ASC->GetActivatableAbilities();
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.AttackCombo")));
				ASC->GetActivatableAbilities();

				for (FGameplayAbilitySpec Spec : ActiveAbilities)
				{
					UAttackCombo* ComboAbility = Cast<UAttackCombo>(Spec.Ability);
					if (ComboAbility)
					{
						ComboAbility->CheckContinueCombo(Character);
					}

				}
			}
		}
	}
}
