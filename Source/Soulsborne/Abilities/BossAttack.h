#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "BossAttack.generated.h"

class UAnimMontage;

DECLARE_DYNAMIC_DELEGATE(FOnTurnFinished);


UCLASS()
class SOULSBORNE_API UBossAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBossAttack();
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Attack")
	UAnimMontage* PrimaryAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack Flow")
	float PauseTimeBetweenAttacks = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Flow")
	float TurnSpeed = 100.0f; // Degrees per second

protected:
	// Called when ability starts
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled
	) override;

	UFUNCTION()
	void OnMontage1Completed();
	UFUNCTION()
	void OnMontage1InterruptedOrCancelled();
	UFUNCTION()
	void OnMontage2Completed();
	UFUNCTION()
	void OnMontage2InterruptedOrCancelled();
	UFUNCTION()
	void OnMontage3Completed();
	UFUNCTION()
	void OnMontage3InterruptedOrCancelled();
	
	UFUNCTION()
	void OnWaitDelayFinishedAfterAttack2();

	// Custom logic triggered during montage
	UFUNCTION()
	virtual void OnAttackHitWindow(FGameplayEventData Payload);

	UFUNCTION()
	void OnWaitDelayFinished();
	
	UFUNCTION()
	void OnTurnToPlayerFinished();

	void StartTurnToPlayer(FOnTurnFinished OnFinishedDelegate);

	bool bPauseDelegate = false;
	bool bUseDesiredRotation = false;
	bool bOrientRotationToMovement = false;
	UPROPERTY()
	ABossCharacter* Boss;
	FDelegateHandle MontageEndedHandle;
	FOnMontageEnded AbilityMontageEnded;
	UPROPERTY()
	AActor* PlayerTarget;
	FTimerHandle TurnTimerHandle;
};
