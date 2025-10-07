// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshSwapNotify.h"

#include "Soulsborne/Characters/BossCharacter.h"


void UMeshSwapNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// Example: Cast to your boss and call an attack function
	if (ABossCharacter* Boss = Cast<ABossCharacter>(Owner))
	{
		if (MeshComp)
		{
			Boss->ChangeMesh(NewMeshComp); // Replace with your own method
			UE_LOG(LogTemp, Log, TEXT("AnimNotify triggered: Boss attack logic executed."));
		}
	}
}
