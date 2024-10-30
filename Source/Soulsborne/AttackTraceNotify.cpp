// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTraceNotify.h"
#include "TimerManager.h"
#include "PlayerCombatComponent.h"
#include "PlayerCombatInterface.h"
#include "PlayerCombatInterface.cpp"

void UAttackTraceNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UPlayerCombatComponent* CombatComponent = Owner->FindComponentByClass<UPlayerCombatComponent>();
		if (CombatComponent)
		{
			//BeginTrace
			CombatComponent->BeginDamageTrace();
			//IPlayerCombatInterface::Execute_StartDamageTrace(Owner);
		}
	}
}

void UAttackTraceNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		UPlayerCombatComponent* CombatComponent = Owner->FindComponentByClass<UPlayerCombatComponent>();
		if (CombatComponent)
		{
			//BeginTrace
			CombatComponent->EndDamageTrace();
			//IPlayerCombatInterface::Execute_EndDamageTrace(Owner);
		}
	}
}