#include "StatBarWidget.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"


void UStatBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Animate damage trailing bar
	
	if (bDamagePending && SecondaryBar)
	{
		DamageTimer += InDeltaTime;

		if (DamageTimer >= DamageFadeDelay)
		{
			const float Current = SecondaryBar->GetPercent();
			const float New = FMath::FInterpTo(Current, TargetProgressPercent, InDeltaTime, DamageDecayRate);

			SecondaryBar->SetPercent(New);

			if (FMath::IsNearlyEqual(New, TargetProgressPercent, 0.01f))
			{
				bDamagePending = false;
			}
		}
	}
}

void UStatBarWidget::UpdateProgress(float NewPercent)
{
	NewPercent = FMath::Clamp(NewPercent, 0.f, 1.f);

	if (NewPercent <= CurrentProgressPercent)
	{
		// Took damage
		if (StatBar)
		{
			StatBar->SetPercent(NewPercent);
		}

		if (SecondaryBar)
		{
			if (SecondaryBar->GetPercent() <= CurrentProgressPercent)
			{
				SecondaryBar->SetPercent(CurrentProgressPercent); // Trail behind
			}
		}

		// Start delay before decay
		DamageTimer = 0.f;
		bDamagePending = true;
	}
	else if (NewPercent > CurrentProgressPercent)
	{
		// Healed
		if (StatBar)
		{
			StatBar->SetPercent(NewPercent);
		}
	}

	// Store new state
	TargetProgressPercent = NewPercent;
	CurrentProgressPercent = NewPercent;
}

