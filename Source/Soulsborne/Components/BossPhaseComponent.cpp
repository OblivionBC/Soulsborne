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
	UE_LOG(LogTemp, Display, TEXT("Phase %d"), phase);
	CurrentPhase = phase;
	OwnerBoss = Cast<ABossCharacter>(GetOwner());
	OnPhaseChanged.Broadcast(CurrentPhase);
	if (OwnerBoss)
	{
		if (ABossAIController* AI = Cast<ABossAIController>(OwnerBoss->GetController()))
		{
			AI->SetPhase(phase);
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
			break;
		}
	}
}
