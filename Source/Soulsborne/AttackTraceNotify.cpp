// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTraceNotify.h"
#include "TimerManager.h"
#include "PlayerCombatComponent.h"
#include "PlayerCombatInterface.h"
#include "PlayerCombatInterface.cpp"
#include "AbilitySystemGlobals.h"
#include "AI/BossAIController.h"
#include "Characters/BossCharacter.h"
#include "Characters/SoulsPlayerCharacter.h"
#include "Components/IBossCombatInterface.h"
#include "Kismet/GameplayStatics.h"

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
			}else if (UPlayerCombatComponent* CombatComponent = Owner->FindComponentByClass<UPlayerCombatComponent>())
			{
				CombatComponent->BeginDamageTrace();
			}
		}
	}
}

void UAttackTraceNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (!Cast<ABossCharacter>(Owner))
		{
			if (UPlayerCombatComponent* CombatComponent = Owner->FindComponentByClass<UPlayerCombatComponent>())
			{
				//BeginTrace
				CombatComponent->EndDamageTrace();
				//IPlayerCombatInterface::Execute_EndDamageTrace(Owner);
			}
		}
	}
}