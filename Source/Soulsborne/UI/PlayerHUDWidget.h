#pragma once

#include "CoreMinimal.h"
#include "InventoryWidget.h"
#include "StatBarWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class SOULSBORNE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Called to update health bar */
	UFUNCTION(BlueprintCallable, Category = "PlayerHUD")
	void UpdateHealthBar(float CurrentHealth, float MaxHealth);
	void UpdateStaminaBar(float CurrentStamina, float MaxStamina);
	UInventoryWidget* GetInventoryWidget() const;
protected:
	UPROPERTY(meta = (BindWidget))
	UStatBarWidget* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UStatBarWidget* StaminaBar;
	UPROPERTY(meta = (BindWidget))
	UInventoryWidget* Inventory;
	
};
