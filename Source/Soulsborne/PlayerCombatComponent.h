/* PlayerCombatComponent
Combat Component for Players and Player Characters (May be extended to enemys)
Handles Combat Mechanics such as Locking Cameras, Attacks, and Damage Application
Child of Actor Component Class
*/

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "PlayerEquipment.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOULSBORNE_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ClearTimer();
	//Creates Line trace between the two arrows of a PlayerEquipment object and traces for Actors with Ability System Component
	void DamageTrace();
	//Called by the PlayerCharacter when starting a Trace Period of an attack animation
	void BeginDamageTrace(AActor* RHandArmament);
	void BeginDamageTrace();
	//Called by the PlayerCharacter when ending a Trace Period of an attack animation
	void EndDamageTrace();
	//Applies damage via Gameplay Effect
	void ApplyDamage(AActor* Target, float Damage);
	//Traces for actor, and adds them to the TargetLockActor property, which is checked in the Tick function to lock the players camera
	void TargetLockCamera();
	void TakeDamage();

	UPROPERTY()
	APlayerEquipment* Weapon;

	UPROPERTY()
	ABaseCharacter* Owner;

	UPROPERTY()
	AActor* CameraLockActor;

	UPROPERTY()
	AActor* TargetLockIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollForwardMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* TakeDamageMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	UClass* TargetLockIconClass;

	FCollisionQueryParams DamageTraceCollisionParams;
	FTimerHandle TraceTimerHandle;

};
