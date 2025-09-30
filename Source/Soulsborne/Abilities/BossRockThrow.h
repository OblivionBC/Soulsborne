#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "Soulsborne/ENUM/BossPhaseENum.h"
#include "BossRockThrow.generated.h"

UCLASS()
class SOULSBORNE_API UBossRockThrow : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBossRockThrow();

	UPROPERTY(EditDefaultsOnly, Category = "Attack Flow")
	float PauseTimeBetweenAttacks = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Flow")
	float TurnSpeed = 100.0f; // Degrees per second

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackMontage;
protected:
	// Called when ability starts
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	UFUNCTION()
	void PlayRipMontage();
	UFUNCTION()
	void PlayThrowMontage();
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility, bool bWasCancelled
	) override;
	UFUNCTION()
	void EndAbility();
	UFUNCTION()
	void OnSpawnRock(FGameplayEventData Payload);
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	

	UFUNCTION()
	void Strafe();
	void PerformThrow();
	UFUNCTION()
	void OnMontage2Completed();
	
	UFUNCTION()
	virtual void OnAttackHitWindow(FGameplayEventData Payload);
	UFUNCTION()
	void Interupt();

	UPROPERTY(EditAnywhere, Category = "Rock")
	FName AttachSocketName = "Emote_Skull";

	UPROPERTY(EditAnywhere, Category = "Rock")
	float ThrowDelay = 1.0f; // seconds before throw

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StrafeChance = 0.6f;

	FSimpleDelegate TurnFinished;
	EBossPhase OldPhase;
	UPROPERTY()
	ABossAIController* BossAIController = nullptr;
	UPROPERTY()
	AProjectile* CachedRock = nullptr;
	UPROPERTY()
	ABossCharacter* Boss;
	UPROPERTY()
	AActor* PlayerTarget;
	FTimerHandle TurnTimerHandle;

	bool bIsThrown = false;
	bool bIsStrafing = false;
	bool bIsInAnimation = false;
};
