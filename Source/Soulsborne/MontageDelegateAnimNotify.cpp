// Fill out your copyright notice in the Description page of Project Settings.


#include "MontageDelegateAnimNotify.h"

void UMontageDelegateAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		// Custom logic here
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify triggered!"));
	}
}