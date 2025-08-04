#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.generated.h"

UCLASS()
class SOULSBORNE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	UFUNCTION()
	void OnFire(const FVector& Direction);
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Impulse")
	URadialForceComponent* RadialForce;

	UPROPERTY(VisibleAnywhere)
	UStaticMesh* StaticMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactEmitter;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float Speed = 150.f;
	UPROPERTY(EditAnywhere, Category = "Trace")
	float Gravity = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceDistance = 150.f;
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float Damage = 20.f;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
public:
	virtual void Tick(float DeltaTime) override;
};
