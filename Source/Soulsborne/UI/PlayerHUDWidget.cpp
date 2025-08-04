#include "PlayerHUDWidget.h"
#include "Components/ProgressBar.h"

void UPlayerHUDWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0)
	{
		const float Percent = CurrentHealth / MaxHealth;
		HealthBar->UpdateProgress(Percent);
	}
}

void UPlayerHUDWidget::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0)
	{
		const float Percent = CurrentStamina / MaxStamina;
		StaminaBar->UpdateProgress(Percent);
	}
}