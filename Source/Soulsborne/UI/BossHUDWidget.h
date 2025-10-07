#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUDWidget.generated.h"

UCLASS()
class SOULSBORNE_API UBossHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Called to update health bar */
	UFUNCTION(BlueprintCallable, Category = "Boss HUD")
	void UpdateHealthBar(float CurrentHealth, float MaxHealth);

	/** Set boss name */
	UFUNCTION(BlueprintCallable, Category = "Boss HUD")
	void SetBossName(const FText& Name);

	/** Update phase indicator */
	UFUNCTION(BlueprintCallable, Category = "Boss HUD")
	void SetPhase(int32 Phase);

	/** Optional enrage FX trigger */
	UFUNCTION(BlueprintCallable, Category = "Boss HUD")
	void ShowEnrageFX(bool bShow);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* PhaseText;

	UPROPERTY(meta = (BindWidgetOptional))
	class UImage* EnrageFX;
};
