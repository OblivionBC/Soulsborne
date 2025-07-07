#include "BossCharacter.h"

#include "AIController.h"
#include "SoulsPlayerCharacter.h"
#include "../Abilities/AttackCombo.h"
#include "../GameplayTags/SoulsGameplayTags.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Soulsborne/Abilities/BossAttack.h"

ABossCharacter::ABossCharacter()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage(TEXT("/Game/Soulsbourne/Animations/Attacks/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));
	PhaseComponent = CreateDefaultSubobject<UBossPhaseComponent>(TEXT("PhaseComponent"));
	HUDComponent = CreateDefaultSubobject<UBossHUDComponent>(TEXT("HUDComponent"));
	if (Montage.Succeeded())
	{
		EnrageMontage = Montage.Object.Get();
	}
	if (PhaseComponent)
	{
		PhaseComponent->OnPhaseChanged.AddDynamic(this, &ABossCharacter::HandlePhaseChange);
	}
	
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Boss_Rampage);
	double Health = 0.0;
	GetHealth_Implementation(Health);
	HUDComponent->Initialize(FText::FromString("Rampage"), Health);
	BehaviorTree = LoadObject<UBehaviorTree>(nullptr, TEXT("/Game/AI/BT_BossCPP.BT_BossCPP"));
}

void ABossCharacter::OnDeath()
{
	Super::OnDeath();
}

void ABossCharacter::GiveDefaultAbilities()
{
	Super::GiveDefaultAbilities();
	// Grant abilities, but only on the server
	if (HasAuthority() && AbilitySystemComponent)
	{
		//C++ Implemented Abilities
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UBossAttack::StaticClass(), 1, 0));
	}
}

void ABossCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType)
{
	Super::SoulsTakeDamage(DamageAmount, DamageType);
	double Health = 0.0;
	UE_LOG(LogTemp, Display, TEXT("DamageAmount: %f"), DamageAmount);
	GetHealth_Implementation(Health);
	float HealthPercent = Health / this->MaxHealth;
	PhaseComponent->CheckPhaseTransition(HealthPercent);
	HUDComponent->UpdateHealth(Health);

}

void ABossCharacter::OnPlayerKilledHandler(AActor* KilledPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy AI detected player was killed!"));
	this->GetMesh()->SetSimulatePhysics(true);
}


void ABossCharacter::HandlePhaseChange(int32 NewPhase)
{
	// Play enrage montage or apply gameplay effect
	UE_LOG(LogTemp, Display, TEXT("PhaseChange: %d"), NewPhase);
	if (NewPhase == 1 && EnrageMontage)
	{
		PlayAnimMontage(EnrageMontage);
	}
}

void ABossCharacter::PerformBasicAttack()
{
	float Radius = 200.f;
	float Distance = 150.f;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector Start = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector End = Start + (Forward * Distance);

	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		QueryParams
	);

	DrawDebugCapsule(
	World,
	(Start + End) * 0.5f, // Midpoint of sweep
	(End - Start).Size() * 0.5f, // Half-height of capsule
	Sphere.GetSphereRadius(),
	FRotationMatrix::MakeFromZ(End - Start).ToQuat(),
	FColor::Green,
	false,
	1.0f);
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			ASoulsPlayerCharacter* Character = Cast<ASoulsPlayerCharacter>(HitActor);
			if (!Character)
				continue;
			
			UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
			if (!TargetASC)
				continue;

			Character->SoulsTakeDamage(30.f, TEXT("Slash"));
		}
	}
}

void ABossCharacter::PerformPhaseAbility(int32 Phase)
{
	// For now reuse combo; later swap this with other abilities based on phase
	if (Phase != 0)
	{
		PerformBasicAttack(); // placeholder
	}
}

void ABossCharacter::TraceForAttack(FGameplayTag AttackTag)
{
	FName AttackName = AttackTag.GetTagName();
	UE_LOG(LogTemp, Display, TEXT("PerformBasicAttack"))
	if (AttackName == TEXT("Boss.Attack"))
	{
		PerformBasicAttack();
	}
}