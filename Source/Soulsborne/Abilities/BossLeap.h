#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "Soulsborne/ENUM/BossPhaseENum.h"
#include "BossLeap.generated.h"

UCLASS()
class SOULSBORNE_API UBossLeap : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBossLeap();

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AirLoopMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* ImpactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* JumpMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ShockwaveFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* LandedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds")
	USoundBase* ImpactSound;
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void PerformLeap();
	void OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnBossLanded(const FHitResult& Hit);

private:
	UPROPERTY()
	ABossCharacter* Boss;

	UPROPERTY()
	AActor* PlayerTarget;

	UPROPERTY()
	ABossAIController* BossAIController;
};

