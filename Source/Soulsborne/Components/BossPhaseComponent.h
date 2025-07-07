#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossPhaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossPhaseChanged, int32, NewPhase);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOULSBORNE_API UBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBossPhaseComponent();
	virtual void BeginPlay() override;
	void SetPhase(int phase);
	void CheckPhaseTransition(float CurrentHealthPercent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phases")
	TArray<float> PhaseThresholds;

	UPROPERTY(BlueprintReadOnly, Category = "Phases")
	int32 CurrentPhase;

	UPROPERTY(BlueprintAssignable)
	FOnBossPhaseChanged OnPhaseChanged;

private:
	UPROPERTY()
	class ABossCharacter* OwnerBoss;
};
