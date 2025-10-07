// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTraceNotify.h"
#include "TimerManager.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "Soulsborne/Characters/SoulsPlayerCharacter.h"

void UAttackTraceNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		{
			if (USoundBase * AttackSound = Owner->BaseAttackSound)
			{
				UGameplayStatics::PlaySoundAtLocation(Owner, AttackSound, Owner->GetActorLocation());
			}
			if (Cast<ABossCharacter>(Owner))
			{
				FGameplayEventData EventData;
				EventData.Instigator = Owner;
				EventData.EventTag = EventTag;

				ASC->HandleGameplayEvent(EventTag, &EventData);
			}else if (ASoulsPlayerCharacter* PlayerCharacter = Cast<ASoulsPlayerCharacter>(Owner))
			{
				PlayerCharacter->StartDamageTrace_Implementation();
			}
		}
	}
}

void UAttackTraceNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}