#include "BossRockThrow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Kismet/GameplayStatics.h" 
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Animations/BossAnimInstance.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"


UBossRockThrow::UBossRockThrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.RockThrow")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.RockThrow")));
}

void UBossRockThrow::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
	Boss = Cast<ABossCharacter>(ActorInfo->AvatarActor.Get());
	if (!Boss || !RipNTossMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Attacking"));

	UAnimInstance* AnimInstance = Boss->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	BossAIController = Cast<ABossAIController>(Boss->GetController());
	
	TurnFinished.BindUObject(this, &UBossRockThrow::PlayRipMontage);
	Boss->RotationComponent->StartSmoothTurnTo(PlayerTarget, 300.0f, TurnFinished);
	
	UAbilityTask_WaitGameplayEvent* SpawnEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.SpawnRock"),
		nullptr,
		true,  // OnlyTriggerOnce
		true   // OnlyMatchExact
	);

	if (SpawnEvent)
	{
		SpawnEvent->EventReceived.AddDynamic(this, &UBossRockThrow::OnSpawnRock);
		SpawnEvent->ReadyForActivation();
	}
	
	UAbilityTask_WaitGameplayEvent* ThrowEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.RockThrow"),
		nullptr,
		true,  // OnlyTriggerOnce
		true   // OnlyMatchExact
	);

	if (ThrowEvent)
	{
		ThrowEvent->EventReceived.AddDynamic(this, &UBossRockThrow::OnAttackHitWindow);
		ThrowEvent->ReadyForActivation();
	}
	Boss->GetCharacterMovement()->MaxWalkSpeed = 400.0f;

}

void UBossRockThrow::PlayRipMontage()
{
	if (!bIsInAnimation)
	{
		bIsInAnimation = true;
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        		this,
        		TEXT("PlayRipTask"),
        		RipNTossMontage,
        		0.7f,
        		FName("Rip"),
        		true,
        		1.0f
        	);
        	if (PlayMontageTask)
        	{
        		if (ThrowSound) UGameplayStatics::PlaySoundAtLocation(Boss->GetWorld(), ThrowSound, Boss->GetActorLocation(), Boss->GetActorRotation());
        		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossRockThrow::Strafe);
        		PlayMontageTask->ReadyForActivation();
        	}
        	else
        	{
        		EndAbility();
        	}
	}
	else
	{
		EndAbility();
	}
	
}

void UBossRockThrow::PlayThrowMontage()
{
	if (!bIsInAnimation)
	{
		bIsInAnimation = true;
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			TEXT("PlayTossTask"),
			RipNTossMontage,
			1.0f,
			FName("Toss"),
			false,
			1.0f
		);
		if (PlayMontageTask)
		{
			PlayMontageTask->OnCompleted.AddDynamic(this, &UBossRockThrow::OnMontage2Completed);

			PlayMontageTask->ReadyForActivation();
		}
		else
		{
			EndAbility();
		}
	}
	else
	{
		EndAbility();
	}
}

void UBossRockThrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Attacking"));
	}
	
	if (BossAIController)
	{
		BossAIController->SetIsAttacking(false);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && RipNTossMontage)
	{
		ASC->StopMontageIfCurrent(*RipNTossMontage);
	}
	
	if (Boss)
	{
		if (Boss->GetMesh())
        {
        	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance());
        	if (AnimInst && RipNTossMontage)
        	{
        		AnimInst->BossPhase = OldPhase;
        		if (AnimInst->bIsHoldingRock)
        		{
        			if (CachedRock)
        			{
        				CachedRock->Destroy();
        			}
        		}
        		AnimInst->bIsHoldingRock = false;
        		AnimInst->Montage_Stop(0.f);
        	}
        }
	    if (UCharacterMovementComponent* movement = Boss->GetCharacterMovement())
	    {
	        movement->bOrientRotationToMovement = false;
	        movement->MaxWalkSpeed = 600.0f;
	    }
	}
	
	bIsThrown = false;
	bIsInAnimation = false;
	RipNTossMontage = nullptr;
	Boss = nullptr;
	PlayerTarget = nullptr;
}

void UBossRockThrow::EndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBossRockThrow::OnSpawnRock(FGameplayEventData Payload)
{
	if (!Boss) return EndAbility();

	CachedRock = Boss->GetWorld()->SpawnActor<AProjectile>(RockProjectile, Boss->GetActorLocation()+100, FRotator::ZeroRotator);
	CachedRock->SetActorEnableCollision(false);
	CachedRock->SetOwner(Boss);
	if (!CachedRock) return;
	CachedRock->CollisionComponent->MoveIgnoreActors.Add(Boss);
	CachedRock->AttachToComponent(Boss->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);

	// Set animation state via AnimInstance
	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->BossPhase = EBossPhase::RockThrow;
		AnimInst->bIsHoldingRock = true;
	}
}

void UBossRockThrow::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	BossAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBossRockThrow::OnMoveCompleted);
	bIsStrafing = false;
	Strafe();
}

void UBossRockThrow::OnAttackHitWindow(FGameplayEventData Payload)
{
	if (!Boss || !CachedRock || !PlayerTarget) return EndAbility();
	FVector LaunchVelocity;
	FVector StartLocation = Boss->GetMesh()->GetSocketLocation(AttachSocketName);
	FVector TargetLocation = PlayerTarget->GetActorLocation();
	StartLocation.Z += 200.f;
	TargetLocation.Z += 200.f;
	FVector TossDirection = FVector::ZeroVector;
	bool bHasSolution = UGameplayStatics::SuggestProjectileVelocity(
		CachedRock->GetWorld(),
		LaunchVelocity,
		StartLocation,
		TargetLocation,
		2500.f,
		false,
		0.0f,
		0.0f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);
	if (bHasSolution)
	{
		CachedRock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CachedRock->OnFire(LaunchVelocity);
	}else
	{
		StartLocation = Boss->GetMesh()->GetSocketLocation(AttachSocketName);
		TargetLocation = PlayerTarget->GetActorLocation();
		TargetLocation.Z += 200.f;
		FVector Direction = TargetLocation - StartLocation;
		float DistanceXY = Direction.Size2D();
		float HeightDiff = TargetLocation.Z - StartLocation.Z;
		float Gravity = FMath::Abs(GetWorld()->GetGravityZ());
		float Speed = 2500.f;

		float Time = DistanceXY / Speed;

		if (Time <= KINDA_SMALL_NUMBER)
		{
			PerformThrow();
			return;
		}

		float VerticalVelocity = (HeightDiff + 0.5f * Gravity * Time * Time) / Time;

		FVector Velocity = Direction.GetSafeNormal2D() * Speed;
		Velocity.Z = VerticalVelocity;
		CachedRock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CachedRock->SetActorEnableCollision(true);
		CachedRock->SetActorTickEnabled(true);
		CachedRock->OnFire(Velocity);
	}


	if (UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance()))
	{
		AnimInst->BossPhase = OldPhase;
		AnimInst->bIsHoldingRock = false;
	}
}

void UBossRockThrow::Interupt()
{
	if (!bIsInAnimation)
	{
		PerformThrow();
	}
}

void UBossRockThrow::Strafe()
{
	if (!Boss || !CachedRock || !PlayerTarget || !BossAIController) return EndAbility();
	URotationComponent* rotcomp = Boss->RotationComponent;
	if (rotcomp)
		rotcomp->LockOnTarget(PlayerTarget, 300.0f);
	if (float Rand = FMath::FRand(); Rand < StrafeChance)
	{
		if (bIsStrafing) return;
		bIsStrafing = true;
		FVector BossLocation = Boss->GetActorLocation();
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (!NavSys || !Boss) return PerformThrow();
		FVector PlayerLocation;
		StrafeChance *= 0.5f;
		FNavLocation RandomLocation;
		const float StrafeRadius = 500.f;

		bool bFound = NavSys->GetRandomReachablePointInRadius(
			Boss->GetActorLocation(),
			StrafeRadius,
			RandomLocation,
			nullptr // default nav data
		);
		if (bFound)
		{
			FRotator LookAtRotation = (PlayerLocation - BossLocation).Rotation();
			Boss->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
			BossAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBossRockThrow::OnMoveCompleted);
			BossAIController->MoveToLocation(RandomLocation.Location);
			BossAIController->ReceiveMoveCompleted.AddDynamic(this, &UBossRockThrow::OnMoveCompleted);
		}else
		{
			bIsStrafing = false;
			bIsInAnimation = false;
			PerformThrow();
		}
		
	}else
	{
		bIsStrafing = false;
		bIsInAnimation = false;
		PerformThrow();
	}
}

void UBossRockThrow::PerformThrow()
{
	if (!Boss || !CachedRock || !BossAIController) return EndAbility();;
	URotationComponent* rotcomp = Boss->RotationComponent;
	if (rotcomp)
		rotcomp->StopLockOn();
	TurnFinished.BindUObject(this, &UBossRockThrow::PlayThrowMontage);
	rotcomp->StartSmoothTurnTo(PlayerTarget, 400.0f, TurnFinished);
	
}

void UBossRockThrow::OnMontage2Completed()
{
	bIsInAnimation = false;
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks * .05);
	WaitDelayTask->OnFinish.AddDynamic(this, &UBossRockThrow::EndAbility);
	WaitDelayTask->ReadyForActivation();
}