#include "BossLeap.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/Characters/BossCharacter.h"
#include "DrawDebugHelpers.h"

UBossLeap::UBossLeap()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Boss.Leap")));
    ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking")));
}

void UBossLeap::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    Boss = Cast<ABossCharacter>(ActorInfo->AvatarActor.Get());
    PlayerTarget = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!Boss || !PlayerTarget)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    BossAIController = Cast<ABossAIController>(Boss->GetController());
    if (BossAIController)
    {
        BossAIController->SetIsAttacking(true);
    }

    UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, 0.0f);
    if (WaitTask)
    {
        WaitTask->OnFinish.AddDynamic(this, &UBossLeap::PerformLeap);
        WaitTask->ReadyForActivation();
    }
}

void UBossLeap::PerformLeap()
{
    UE_LOG(LogTemp, Warning, TEXT("BossLeap"));
    if (!Boss || !PlayerTarget) return;

    FVector BossLoc = Boss->GetActorLocation();
    FVector TargetLoc = PlayerTarget->GetActorLocation();
    float GravityZ = GetWorld()->GetGravityZ() * Boss->GetCharacterMovement()->GravityScale; 
    FVector Direction = (TargetLoc - BossLoc).GetSafeNormal();
    Direction.Z = 0.f;
    float FlightTime = 1.2f;
    
    FVector LaunchVelocity;
    FVector ToTarget = TargetLoc - BossLoc;
    LaunchVelocity.X = ToTarget.X / FlightTime;
    LaunchVelocity.Y = ToTarget.Y / FlightTime;
    LaunchVelocity.Z = (ToTarget.Z - 0.5f * GravityZ * FlightTime * FlightTime) / FlightTime;
    if (Boss->JumpFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Boss->JumpFX, BossLoc, FRotator::ZeroRotator);
    }
    if (Boss->JumpSound)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Boss->JumpSound, BossLoc, Boss->GetActorRotation());
    }

    if (JumpMontage)
    {
        Boss->PlayAnimMontage(JumpMontage);
    }

    Boss->LaunchCharacter(LaunchVelocity, true, true);
    Boss->LandedDelegate.AddDynamic(this, &UBossLeap::OnBossLanded);
}

void UBossLeap::OnBossLanded(const FHitResult& Hit)
{
    UE_LOG(LogTemp, Warning, TEXT("BossLanded"));
    if (!Boss) return;

    if (AirLoopMontage)
    {
        Boss->StopAnimMontage(AirLoopMontage);
    }
    
    if (ImpactMontage)
    {
        Boss->PlayAnimMontage(ImpactMontage, 1.0f);
    }

    if (LandedSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this->GetWorld(), LandedSound, Boss->GetActorLocation(), Boss->GetActorRotation());
    }

    if (ShockwaveFX)
    {
        FVector Scale = FVector(2.0f);
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShockwaveFX, Boss->GetActorLocation(), FRotator::ZeroRotator, Scale, true);
    }

    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this->GetWorld(), ImpactSound, Boss->GetActorLocation(), Boss->GetActorRotation());
    }

    FVector Center = Boss->GetActorLocation();
    float Radius = 600.f;

    TArray<FOverlapResult> HitResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

    bool bHit = GetWorld()->OverlapMultiByChannel(
        HitResults,
        Center,
        FQuat::Identity,
        ECC_Pawn,
        Sphere
    );

    TSet<ABaseCharacter*> HitActors;
    if (bHit)
    {
        for (FOverlapResult& Result : HitResults)
        {
            if (ABaseCharacter* HitActor = Cast<ABaseCharacter>(Result.GetActor()))
            {
                if (HitActor != Boss && !HitActors.Contains(HitActor))
                {
                    HitActor->SoulsTakeDamage(30, EDamageType::Blunt);
                    HitActors.Add(HitActor);
                }
            }
        }
    }
    
    Boss->LandedDelegate.RemoveAll(this);
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBossLeap::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    UE_LOG(LogTemp, Warning, TEXT("BossAbilityEnded"));
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Attacking"));
    }

    if (BossAIController)
    {
        BossAIController->SetIsAttacking(false);
    }

    Boss = nullptr;
    PlayerTarget = nullptr;
    BossAIController = nullptr;
}
