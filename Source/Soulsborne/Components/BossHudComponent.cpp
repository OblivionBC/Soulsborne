#include "BossHudComponent.h"

#include "Blueprint/UserWidget.h"

void UBossHUDComponent::Initialize(const FText BossName, const float MaxHealth)
{
	if (!WidgetInstance)
	{
		WidgetInstance = CreateWidget<UBossHUDWidget>(GetWorld(), WidgetClass);

		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
			WidgetInstance->SetBossName(BossName);
			SetMaxHealth(MaxHealth);
			WidgetInstance->UpdateHealthBar(MaxHealth, MaxHealth);
			WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UBossHUDComponent::SetBossHudVisible(bool toggle){
	if (WidgetInstance)
	{
		if (toggle)
		{
			WidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}else
		{
			WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UBossHUDComponent::SetMaxHealth(const float InMaxHealth)
{
	BossMaxHealth = InMaxHealth;

	if (WidgetInstance)
	{
		WidgetInstance->UpdateHealthBar(InMaxHealth, InMaxHealth); // full bar
	}
}

void UBossHUDComponent::UpdateHealth(const float CurrentHealth)
{
	if (WidgetInstance)
	{
		WidgetInstance->UpdateHealthBar(CurrentHealth, BossMaxHealth);
	}
}

void UBossHUDComponent::SetPhase(int32 Phase)
{
	if (WidgetInstance)
	{
		WidgetInstance->SetPhase(Phase);
	}
}

void UBossHUDComponent::SetEnrageActive(const bool bActive)
{
	if (WidgetInstance)
	{
		WidgetInstance->ShowEnrageFX(bActive);
	}
}
