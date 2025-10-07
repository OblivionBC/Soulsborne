// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HealNotify.generated.h"

/**
 * 
 */
UCLASS()
class SOULSBORNE_API UHealNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* ParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* SoundEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float HealAmount = 20.0f;
};
