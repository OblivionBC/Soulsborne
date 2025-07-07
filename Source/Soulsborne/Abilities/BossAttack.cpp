#include "BossAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h" 
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Characters/BossCharacter.h"


UBossAttack::UBossAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.Attack")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.Attack")));


	static ConstructorHelpers::FObjectFinder<UAnimMontage> BossMont(TEXT("/Game/ParagonRampage/Characters/Heroes/Rampage/Animations/Attack_Biped_Melee_A_Montage1.Attack_Biped_Melee_A_Montage1"));
	if (BossMont.Succeeded())
	{
		AttackMontage = BossMont.Object;
	}
}

void UBossAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayerTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
	Boss = Cast<ABossCharacter>(ActorInfo->AvatarActor.Get());
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
	UE_LOG(LogTemp, Display, TEXT("Activating Attack"));
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayAttack1Task"), // Unique task name
		AttackMontage,
		1.0f, // Rate
		FName("Attack1"), // Start Section Name
		true, // bStopWhenAbilityEnds
		1.0f // Anim Root Motion Translation Scale
	);
	if (PlayMontageTask)
	{
		// Bind directly to your UFUNCTION callbacks
		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossAttack::OnMontage1Completed);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBossAttack::OnMontage1InterruptedOrCancelled);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UBossAttack::OnMontage1InterruptedOrCancelled);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UBossAttack::OnMontage1Completed);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return; // Important: return after ending ability
	}

	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, Boss->GetActorLocation());
	}
	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.AttackWindow"),
		nullptr,
		false,  // OnlyTriggerOnce
		true   // OnlyMatchExact
	);

	if (WaitEvent)
	{
		WaitEvent->EventReceived.AddDynamic(this, &UBossAttack::OnAttackHitWindow);
		WaitEvent->ReadyForActivation();
	}
}

void UBossAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Display, TEXT("Ending Attack"));
	if (bIsActive)
	{
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Attacking"));
	if (ABossAIController * ai = Cast<ABossAIController>(Boss->GetController()))
	{
		ai->SetIsAttacking(false);
	}
	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
	if (Boss && AttackMontage)
	{
		UAnimInstance* AnimInstance = Boss->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Stop(0.f, AttackMontage);
			//AnimInstance->OnMontageEnded.Clear();
		}
	}
	if (Boss && Boss->GetCharacterMovement())
	{
		Boss->GetCharacterMovement()->bUseControllerDesiredRotation = useDesiredRotation;
		Boss->GetCharacterMovement()->bOrientRotationToMovement = orientRotationToMovement;
	}
	Boss = nullptr;
	PlayerTarget = nullptr;
}

void UBossAttack::OnAttackHitWindow(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack hit window triggered!"));
	if (IBossCombatInterface* Interface = Cast<IBossCombatInterface>(GetAvatarActorFromActorInfo()))
	{
		Interface->TraceForAttack(FGameplayTag::RequestGameplayTag(FName("Boss.Attack")));
	}
}

void UBossAttack::StartTurnToPlayer(FOnTurnFinished OnFinishedDelegate)
{
	UE_LOG(LogTemp, Display, TEXT("Turning to pLayer"));
    if (!Boss || !PlayerTarget)
    {
        OnFinishedDelegate.ExecuteIfBound();
        return;
    }
	
    if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
    {
    	useDesiredRotation = MoveComp->bUseControllerDesiredRotation;
    	orientRotationToMovement = MoveComp->bOrientRotationToMovement;
        MoveComp->bUseControllerDesiredRotation = false;
        MoveComp->bOrientRotationToMovement = false;
    }
    
    // Set a timer to update rotation gradually
    GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, [this, OnFinishedDelegate]()
    {
    	UE_LOG(LogTemp, Display, TEXT("In Turn Timer"));
        if (!Boss || !PlayerTarget)
        {
            GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
            OnFinishedDelegate.ExecuteIfBound();
            return;
        }

        FRotator CurrentRotation = Boss->GetActorRotation();
        FVector LookAtLocation = PlayerTarget->GetActorLocation();
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Boss->GetActorLocation(), LookAtLocation);

        // Interpolate rotation
        float DeltaTime = GetWorld()->GetDeltaSeconds();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnSpeed);
        Boss->SetActorRotation(NewRotation);

        // Check if we are close enough to the target rotation
        if (FMath::Abs(FRotator::NormalizeAxis(NewRotation.Yaw - TargetRotation.Yaw)) < 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
            
            // Re-enable controller rotation
            if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
            {
                MoveComp->bUseControllerDesiredRotation = useDesiredRotation;
                MoveComp->bOrientRotationToMovement = orientRotationToMovement;
            }

            OnFinishedDelegate.ExecuteIfBound();
        }
    }, GetWorld()->GetDeltaSeconds(), true);
}

void UBossAttack::OnMontage1Completed()
{
    // Attack 1 finished. Now, pause.
	UE_LOG(LogTemp, Display, TEXT("Montage 1 COmpleted"));
	if (!pauseDelegate)
	{
		pauseDelegate = true;
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks);
        WaitDelayTask->OnFinish.AddDynamic(this, &UBossAttack::OnWaitDelayFinished);
        WaitDelayTask->ReadyForActivation();
	}
}

void UBossAttack::OnMontage1InterruptedOrCancelled()
{
	if (!pauseDelegate)
	{
		pauseDelegate = true;
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks);
		WaitDelayTask->OnFinish.AddDynamic(this, &UBossAttack::OnWaitDelayFinished);
		WaitDelayTask->ReadyForActivation();
	}
}

void UBossAttack::OnMontage2Completed()
{
	UE_LOG(LogTemp, Display, TEXT("Montage 2 COmpleted"));
	// Attack 2 finished. Another pause.
	if (!pauseDelegate)
	{
		pauseDelegate = true;
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks * .05);
		WaitDelayTask->OnFinish.AddDynamic(this, &UBossAttack::OnWaitDelayFinishedAfterAttack2);
		WaitDelayTask->ReadyForActivation();
	}
}
void UBossAttack::OnMontage2InterruptedOrCancelled()
{
	// Attack 1 finished. Now, pause.
	if (!pauseDelegate)
	{
		pauseDelegate = true;
		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, PauseTimeBetweenAttacks);
		WaitDelayTask->OnFinish.AddDynamic(this, &UBossAttack::OnWaitDelayFinishedAfterAttack2);
		WaitDelayTask->ReadyForActivation();
	}
}

void UBossAttack::OnMontage3Completed()
{
	UE_LOG(LogTemp, Display, TEXT("Montage 3 COmpleted"));
	// This is the end of the entire sequence.
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBossAttack::OnMontage3InterruptedOrCancelled()
{
	UE_LOG(LogTemp, Display, TEXT("Montage 3 interrupted"));
	// Attack 1 finished. Now, pause.
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBossAttack::OnWaitDelayFinished()
{
    // Pause finished. Now, turn to face the player.
	UE_LOG(LogTemp, Display, TEXT("Waiting Delay Finished"));
    FOnTurnFinished TurnFinishedDelegate;
    TurnFinishedDelegate.BindDynamic(this, &UBossAttack::OnTurnToPlayerFinished);
    StartTurnToPlayer(TurnFinishedDelegate);
}

void UBossAttack::OnTurnToPlayerFinished()
{
	UE_LOG(LogTemp, Display, TEXT("OnTurnToPlayerFinished"));
    // Turn finished. Now, play Attack 2.
	pauseDelegate = false;
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayAttack2Task"), // Unique task name
		AttackMontage,
		0.9f,
		FName("Attack2"),
		true,
		1.0f
	);

	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossAttack::OnMontage2Completed);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBossAttack::OnMontage2InterruptedOrCancelled);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UBossAttack::OnMontage2InterruptedOrCancelled);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UBossAttack::OnMontage2Completed);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UBossAttack::OnWaitDelayFinishedAfterAttack2()
{
	UE_LOG(LogTemp, Display, TEXT("OnWaitDelayFinishedAfterAttack2"));
	pauseDelegate = false;
	// Pause finished. Now, play Attack 3.
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayAttack3Task"), // Unique task name
		AttackMontage,
		0.7f,
		FName("Attack3"),
		true,
		1.0f
	);

	if (PlayMontageTask)
	{
		PlayMontageTask->OnCompleted.AddDynamic(this, &UBossAttack::OnMontage3Completed);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UBossAttack::OnMontage3InterruptedOrCancelled);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UBossAttack::OnMontage3InterruptedOrCancelled);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UBossAttack::OnMontage3Completed);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
