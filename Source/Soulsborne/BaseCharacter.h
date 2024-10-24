// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SOULSBORNE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	UAbilitySystemComponent *  GetAbilitySystemComponent();
protected:
	// Called when the game starts or when spawned
	UAbilitySystemComponent* AbilitySystemComponent;

};
