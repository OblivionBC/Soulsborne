#include "Projectile.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "BaseCharacter.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	InitialLifeSpan = 0.0f;
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	StaticMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("StaticMesh"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	RootComponent = CollisionComponent;
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->SetGenerateOverlapEvents(true); 
	CollisionComponent->SetSimulatePhysics(false);

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->SetupAttachment(RootComponent);
	
	RadialForce->bImpulseVelChange = true;
	RadialForce->bAutoActivate = false;
	RadialForce->bIgnoreOwningActor = true;
	
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
	ProjectileMovementComponent->bAutoActivate = false;
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
}

void AProjectile::OnFire(const FVector& Direction)
{
	ProjectileMovementComponent->Velocity = Direction;
	FRotator Rotation = Direction.Rotation();
	SetActorRotation(Rotation);
	SetActorEnableCollision(true);
	ProjectileMovementComponent->Activate();
	FTimerHandle TimerHandle;
	/*
	 *GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]() { CollisionComponent->SetGenerateOverlapEvents(true); },
		1.f, // small delay
		false
	);
	*/
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Error, TEXT("Projectile EndPlay: %d"), (int32)EndPlayReason);
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetActorEnableCollision(true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
	RadialForce->Radius = TraceRadius;
	RadialForce->ImpulseStrength = 2000.f;
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ImpactEmitter)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitter, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}

	const FVector TraceStart = Hit.ImpactPoint;
	const FVector End = TraceStart + GetActorForwardVector() * TraceDistance;

	FHitResult TraceHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Start = GetActorLocation();

	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);
	
	RadialForce->FireImpulse();
	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			ABaseCharacter* Character = Cast<ABaseCharacter>(HitActor);
			if (!Character)
				continue;

			Character->SoulsTakeDamage(Damage, EDamageType::Projectile);
		}
	}

	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	RadialForce->FireImpulse();
	Destroy();
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ImpactEmitter)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitter, SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
	}

	const FVector TraceStart = SweepResult.ImpactPoint;
	const FVector End = TraceStart + GetActorForwardVector() * TraceDistance;

	FHitResult TraceHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Start = GetActorLocation();

	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			ABaseCharacter* Character = Cast<ABaseCharacter>(HitActor);
			if (!Character)
				continue;
			
			UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
			if (!TargetASC)
				continue;

			
			Character->SoulsTakeDamage(Damage, EDamageType::Projectile);
		}
	}
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	RadialForce->FireImpulse();
	Destroy();
}
