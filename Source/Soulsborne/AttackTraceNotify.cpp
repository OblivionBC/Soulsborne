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
		if (Owner->GetClass()->ImplementsInterface(UPlayerCombatInterface::StaticClass()))
		{
			//BeginTrace
			IPlayerCombatInterface::Execute_StartDamageTrace(Owner);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Notify Called"));
		}
	}
}

void UAttackTraceNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (Owner->GetClass()->ImplementsInterface(UPlayerCombatInterface::StaticClass()))
		{
			//End Sword Trace
			IPlayerCombatInterface::Execute_EndDamageTrace(Owner);
		}
	}
}