#include "BossCharacter.h"

#include "AIController.h"
#include "SoulsPlayerCharacter.h"
#include "../GameplayTags/SoulsGameplayTags.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Abilities/BossAttack.h"
#include "Animation/PoseSnapshot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimInstance.h"
#include "Soulsborne/Abilities/BossLeap.h"
#include "Components/CapsuleComponent.h"
#include "Soulsborne/Abilities/BossRockThrow.h"
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Animations/BossAnimInstance.h"
#include "Soulsborne/Components/RotationComponent.h"

ABossCharacter::ABossCharacter()
{
	PhaseComponent = CreateDefaultSubobject<UBossPhaseComponent>(TEXT("PhaseComponent"));
	HUDComponent = CreateDefaultSubobject<UBossHUDComponent>(TEXT("HUDComponent"));
	PrimaryActorTick.bCanEverTick = true;
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
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	if (ABossAIController* BossController = Cast<ABossAIController>(GetController()))
	{
		BossAIController = BossController;
		BossController->SetbIsCombatEngaged(false);
	}
	bIsInvulnerable = true;
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
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(BasicAttackAbility, 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(RockThrowAbility, 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(LeapAbility, 1, 0));
	}
}

void ABossCharacter::SoulsTakeDamage(float DamageAmount, EDamageType DamageType)
{
	Super::SoulsTakeDamage(DamageAmount, DamageType);
	if (bIsInvulnerable)
	{
		return;
	}
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
	GetCharacterMovement()->StopMovementImmediately();
	
	if (ABossAIController* BossController = Cast<ABossAIController>(GetController()))
	{
		BossController->SetbIsPlayerDead(true);
	}
	
}

void ABossCharacter::HandlePhaseChange(int32 NewPhase)
{
	
	UE_LOG(LogTemp, Display, TEXT("PhaseChange: %d"), NewPhase);
	if (ABossAIController* AI = Cast<ABossAIController>(this->GetController()))
	{
		AI->SetbIsCombatEngaged(false);
	}
	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(this->GetMesh()->GetAnimInstance());
	if (NewPhase == 1)
	{
		if (AnimInst)
		{
			AnimInst->BossPhase = EBossPhase::Phase1;
		}
	}
	if (NewPhase == 2)
	{
		if (AnimInst)
		{
			AnimInst->BossPhase = EBossPhase::Phase2;
		}
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
	
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			ASoulsPlayerCharacter* Character = Cast<ASoulsPlayerCharacter>(HitActor);
			if (!Character)
				continue;

			Character->SoulsTakeDamage(30.f, EDamageType::Slash);
		}
	}
}

void ABossCharacter::PerformPhaseAbility(int32 Phase)
{
	if (Phase != 0)
	{
		PerformBasicAttack();
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

void ABossCharacter::ChangeMesh(USkeletalMesh* NewMesh)
{
	if (!NewMesh || !GetMesh()) return;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if (!MeshComp->SkeletalMesh || MeshComp->SkeletalMesh->GetSkeleton() != NewMesh->GetSkeleton())
	{
		UE_LOG(LogTemp, Error, TEXT("Skeleton mismatch, cannot swap mesh."));
		return;
	}

	// Set the new mesh but keep the current AnimInstance
	MeshComp->SetSkeletalMeshAsset(NewMesh); // false = do NOT reinitialize the anim instance

	UE_LOG(LogTemp, Log, TEXT("Skeletal mesh swapped and animation preserved."));
}
