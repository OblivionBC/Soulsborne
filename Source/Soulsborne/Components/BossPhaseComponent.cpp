#include "BossPhaseComponent.h"
#include "../Characters/BossCharacter.h"
#include "Soulsborne/AI/BossAIController.h"

UBossPhaseComponent::UBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OwnerBoss = nullptr;
	CurrentPhase = 0;
}

// BossPhaseComponent.cpp
void UBossPhaseComponent::BeginPlay()
{
	Super::BeginPlay();
	PhaseThresholds.Add(1.0);
	PhaseThresholds.Add(.5);
	OwnerBoss = Cast<ABossCharacter>(GetOwner());
	CurrentPhase = 0;
}

void UBossPhaseComponent::SetPhase(int phase)
{
	CurrentPhase = phase;
	OwnerBoss = Cast<ABossCharacter>(GetOwner());
	if (OwnerBoss)
	{
		if (ABossAIController* ai = Cast<ABossAIController>(OwnerBoss->GetController()))
		{
			ai->SetPhase(phase);
		}
	}
}
void UBossPhaseComponent::CheckPhaseTransition(float HealthPercent)
{
 	for (int32 i = CurrentPhase; i < PhaseThresholds.Num(); i++)
	{
		if (HealthPercent <= PhaseThresholds[i])
		{
			SetPhase(i+1);
			OnPhaseChanged.Broadcast(CurrentPhase);
			break;
		}
	}
}
