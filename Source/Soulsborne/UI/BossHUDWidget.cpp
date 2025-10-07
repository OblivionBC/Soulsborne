#include "BossHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
void UBossHUDWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0)
	{
		const float Percent = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(Percent);
	}
}

void UBossHUDWidget::SetBossName(const FText& Name)
{
	if (BossNameText)
	{
		BossNameText->SetText(Name);
	}
}

void UBossHUDWidget::SetPhase(int32 Phase)
{
	if (PhaseText)
	{
		PhaseText->SetText(FText::Format(NSLOCTEXT("BossHUD", "Phase", "Phase {0}"), FText::AsNumber(Phase + 1)));
	}
}

void UBossHUDWidget::ShowEnrageFX(bool bShow)
{
	if (EnrageFX)
	{
		EnrageFX->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
