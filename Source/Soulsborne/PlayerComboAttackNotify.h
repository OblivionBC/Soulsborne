// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "PlayerComboAttackNotify.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API UPlayerComboAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	//Checks if the palyer that notified has the AttackCombo ability active, if so calls the CheckContinueCombo
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
