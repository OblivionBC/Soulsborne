// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SOULSBORNE_API IPlayerCombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void StartDamageTrace() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void EndDamageTrace() const;
};