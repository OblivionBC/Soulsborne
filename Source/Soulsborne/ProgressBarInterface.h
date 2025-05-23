// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProgressBarInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProgressBarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class SOULSBORNE_API IProgressBarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetHealth(double& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetHealthAsRatio(double& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetStamina(double& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetMana(double& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetManaAsRatio(double& Result) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Default")
	void GetStaminaAsRatio(double& Result) const;

};
