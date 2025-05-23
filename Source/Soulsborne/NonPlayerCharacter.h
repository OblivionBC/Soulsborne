// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "PlayerCombatComponent.h"
#include "NonPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class SOULSBORNE_API ANonPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANonPlayerCharacter();
	UBehaviorTree* GetBehaviorTree();
protected:
	virtual void SoulsTakeDamage(float DamageAmount, FName DamageType) override;
	virtual void BeginPlay() override;
	virtual void OnDeath() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GiveDefaultAbilities();
	virtual void PossessedBy(AController* NewController) override;
	virtual void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply);
	virtual void OnRep_PlayerState() override;
	void InitializeAttributes();

	UPlayerCombatComponent* CombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* HitMontage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
};
