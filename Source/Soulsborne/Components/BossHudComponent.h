#pragma once

#include "Soulsborne/UI/BossHUDWidget.h"
#include "BossHudComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSBORNE_API UBossHUDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Initialize(FText BossName, float MaxHealth);
	void SetBossHudVisible(bool toggle);
	void SetMaxHealth(float InMaxHealth);
	void UpdateHealth(float CurrentHealth);
	void SetPhase(int32 Phase);
	void SetEnrageActive(bool bActive);
	UPROPERTY()
	float BossMaxHealth;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Boss HUD")
	TSubclassOf<UBossHUDWidget> WidgetClass;

	UPROPERTY()
	class UBossHUDWidget* WidgetInstance;
};
