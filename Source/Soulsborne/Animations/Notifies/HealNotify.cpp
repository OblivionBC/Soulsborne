// Fill out your copyright notice in the Description page of Project Settings.


#include "HealNotify.h"

#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Characters/SoulsPlayerCharacter.h"

void UHealNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	AActor* Owner = MeshComp->GetOwner();

	if (ParticleEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(Owner->GetWorld(), ParticleEffect, Owner->GetActorLocation());
	}

	if (SoundEffect)
	{
		UGameplayStatics::PlaySoundAtLocation(Owner, SoundEffect, Owner->GetActorLocation());
	}

	// Heal player
	ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(Owner);
	if (Player)
	{
		Player->SoulsHeal(HealAmount);
	}
}
