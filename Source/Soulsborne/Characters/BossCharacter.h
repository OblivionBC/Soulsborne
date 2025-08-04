#pragma once

#include "CoreMinimal.h"
#include "NonPlayerCharacter.h"
#include "Projectile.h"
#include "../Components/BossPhaseComponent.h"
#include "../Components/BossHudComponent.h"
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Components/IBossCombatInterface.h"
#include "Soulsborne/Components/RotationComponent.h"
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
	ABossAIController* BossAIController;

public:
	UFUNCTION()
	void ChangeMesh(USkeletalMesh* NewMesh);
	/** Phase system */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phases")
	UBossPhaseComponent* PhaseComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phases")
	UBossHUDComponent* HUDComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Phases")
	URotationComponent* RotationComponent;

	/** Enrage montage or death cinematic */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* EnrageMontage;

	/** Intro montage (cutscene or intro roar) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* IntroMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* EmoteMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animations")
	UAnimMontage* RipNTossMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FightMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectile> RockProjectile;
};
