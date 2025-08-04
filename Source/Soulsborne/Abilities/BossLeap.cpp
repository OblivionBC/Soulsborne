#include "BossLeap.h"
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


UBossLeap::UBossLeap()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.RockThrow")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.RockThrow")));
}

void UBossLeap::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayerTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
	Boss = Cast<ABossCharacter>(ActorInfo->AvatarActor.Get());
	if (Boss)
	{
		AttackMontage = Boss->RipNTossMontage;
	}
	if (!Boss || !AttackMontage)
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
	
	TurnFinished.BindUObject(this, &UBossLeap::PlayRipMontage);
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
		SpawnEvent->EventReceived.AddDynamic(this, &UBossLeap::OnSpawnRock);
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
		ThrowEvent->EventReceived.AddDynamic(this, &UBossLeap::OnAttackHitWindow);
		ThrowEvent->ReadyForActivation();
	}
	Boss->GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	Boss->GetCharacterMovement()->bOrientRotationToMovement = false;

}

void UBossLeap::PlayRipMontage()
{
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayRipTask"), // Unique task name
		AttackMontage,
		0.7f, // Rate
		FName("Rip"), // Start Section Name
		true, // bStopWhenAbilityEnds
		1.0f // Anim Root Motion Translation Scale
	);
	if (PlayMontageTask)
	{
		// Bind directly to your UFUNCTION callbacks
		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossLeap::Strafe);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBossLeap::Interupt);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility();
	}
}
void UBossLeap::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
	if (ASC && AttackMontage)
	{
		ASC->StopMontageIfCurrent(*AttackMontage);
	}
	
	if (Boss)
	{
		if (Boss->GetMesh())
        {
        	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance());
        	if (AnimInst && AttackMontage)
        	{
        		AnimInst->BossPhase = OldPhase;
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
	
	AttackMontage = nullptr;
	Boss = nullptr;
	PlayerTarget = nullptr;
}

void UBossLeap::EndAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBossLeap::OnSpawnRock(FGameplayEventData Payload)
{
	if (!Boss) return EndAbility();

	CachedRock = Boss->GetWorld()->SpawnActor<AProjectile>(Boss->RockProjectile, Boss->GetActorLocation()+100, FRotator::ZeroRotator);
	CachedRock->SetActorEnableCollision(false);
	if (!CachedRock) return;

	CachedRock->AttachToComponent(Boss->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);

	// Set animation state via AnimInstance
	UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->BossPhase = EBossPhase::RockThrow;
		AnimInst->bIsHoldingRock = true;
	}
}

void UBossLeap::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	Strafe();
}

void UBossLeap::OnAttackHitWindow(FGameplayEventData Payload)
{
	if (!Boss || !CachedRock || !PlayerTarget) return EndAbility();

	FVector StartLocation = Boss->GetMesh()->GetSocketLocation(AttachSocketName);
	FVector TargetLocation = PlayerTarget->GetActorLocation();
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();

	CachedRock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CachedRock->OnFire(Direction);

	if (UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance()))
	{
		AnimInst->BossPhase = OldPhase;
		AnimInst->bIsHoldingRock = false;
	}
}

void UBossLeap::Interupt()
{
	if (!Boss || !CachedRock || !PlayerTarget) return EndAbility();

	FVector StartLocation = Boss->GetMesh()->GetSocketLocation(AttachSocketName);
	FVector TargetLocation = PlayerTarget->GetActorLocation();
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();

	CachedRock->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CachedRock->OnFire(Direction);

	if (UBossAnimInstance* AnimInst = Cast<UBossAnimInstance>(Boss->GetMesh()->GetAnimInstance()))
	{
		AnimInst->BossPhase = OldPhase;
		AnimInst->bIsHoldingRock = false;
	}
}

void UBossLeap::Strafe()
{
	if (!Boss || !CachedRock || !PlayerTarget || !BossAIController) return EndAbility();
	URotationComponent* rotcomp = Boss->RotationComponent;
	if (rotcomp)
		rotcomp->LockOnTarget(PlayerTarget, 300.0f);
	if (float Rand = FMath::FRand(); Rand < StrafeChance)
	{
		FVector BossLocation = Boss->GetActorLocation();
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (!NavSys)
		{
			UE_LOG(LogTemp, Warning, TEXT("Navigation system is NULL"));
			return PerformThrow(); // or fail gracefully
		}
		FVector PlayerLocation;
		StrafeChance *= 0.5f;
		FNavLocation RandomLocation;
		const float StrafeRadius = 500.f;

		if (!NavSys || !Boss) return PerformThrow();

		bool bFound = NavSys->GetRandomReachablePointInRadius(
			Boss->GetActorLocation(),
			StrafeRadius,
			RandomLocation,
			nullptr // default nav data
		);
		
		FRotator LookAtRotation = (PlayerLocation - BossLocation).Rotation();
		Boss->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
		
		BossAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UBossLeap::OnMoveCompleted);
		BossAIController->MoveToLocation(RandomLocation.Location);
		BossAIController->ReceiveMoveCompleted.AddDynamic(this, &UBossLeap::OnMoveCompleted);
	}else
	{
		PerformThrow();
	}
}

void UBossLeap::PerformThrow()
{
	if (!Boss || !CachedRock || !PlayerTarget) return EndAbility();;
	URotationComponent* rotcomp = Boss->RotationComponent;
	if (rotcomp)
		rotcomp->StopLockOn();
	FVector StartLocation = Boss->GetMesh()->GetSocketLocation(AttachSocketName);
	FVector TargetLocation = UGameplayStatics::GetPlayerPawn(Boss, 0)->GetActorLocation();
	FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayTossTask"), // Unique task nameS
		AttackMontage,
		1.0f, // Rate
		FName("Toss"), // Start Section Name
		true, // bStopWhenAbilityEnds
		1.0f // Anim Root Motion Translation Scale
	);
	if (PlayMontageTask)
	{
		// Bind directly to your UFUNCTION callbacks
		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossLeap::OnMontage2Completed);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBossLeap::Interupt);

		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility();
	}
}

void UBossLeap::OnMontage2Completed()
{
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks * .05);
	WaitDelayTask->OnFinish.AddDynamic(this, &UBossLeap::EndAbility);
	WaitDelayTask->ReadyForActivation();
}