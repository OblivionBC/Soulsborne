#pragma once

#include "CoreMinimal.h"
#include "NonPlayerCharacter.h"
#include "../Components/BossPhaseComponent.h"
#include "../Components/BossHudComponent.h"
#include "Soulsborne/Components/IBossCombatInterface.h"
#include "BossCharacter.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API ABossCharacter : public ANonPlayerCharacter, public IBossCombatInterface
{
	GENERATED_BODY()

public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void OnDeath() override;
	virtual void GiveDefaultAbilities() override;
	virtual void SoulsTakeDamage(float DamageAmount, FName DamageType) override;
	virtual void OnPlayerKilledHandler(AActor* KilledPlayer) override;
	UFUNCTION()
	void HandlePhaseChange(int32 NewPhase);
	virtual void PerformBasicAttack() override;
	virtual void PerformPhaseAbility(int32 Phase) override;
	virtual void TraceForAttack(FGameplayTag AttackTag) override;

public:
	/** Phase system */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phases")
	UBossPhaseComponent* PhaseComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phases")
	UBossHUDComponent* HUDComponent;

	/** Enrage montage or death cinematic */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* EnrageMontage;

	/** Intro montage (cutscene or intro roar) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* IntroMontage;

	/** custom boss health bar, sounds, etc. 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	UBossHUDComponent* HUDComponent;*/
};
