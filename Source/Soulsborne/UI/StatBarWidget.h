#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatBarWidget.generated.h"

class UProgressBar;

UCLASS()
class SOULSBORNE_API UStatBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateProgress(float NewHealthPercent);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StatBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SecondaryBar;

	float CurrentProgressPercent = 1.0f;
	float TargetProgressPercent = 1.0f;

	float DamageFadeDelay = 0.8f;
	float DamageDecayRate = 8.0f;

	float RechargeCatchupRate = 1.5f;

	float DamageTimer = 0.f;
	bool bDamagePending = false;
};
