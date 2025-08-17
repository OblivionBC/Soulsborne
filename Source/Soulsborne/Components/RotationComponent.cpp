#include "RotationComponent.h"
#include "Components.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Soulsborne/AI/BossAIController.h"
#include "Soulsborne/AI/Tasks/BTTask_ClearLockOnTarget.h"

URotationComponent::URotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void URotationComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("IsRegistered: %s"), IsRegistered() ? TEXT("true") : TEXT("false"));
	Owner = GetOwner();
}

void URotationComponent::StartSmoothTurnTo(FVector InTargetLocation, float InRotationSpeed, FSimpleDelegate OnFinished)
{
	if (bIsTurning)
	{
		OnFinishedDelegate.ExecuteIfBound();
	}
	
	TargetLocation = InTargetLocation;
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	if (InRotationSpeed == 0.0f)
	{
		RotationSpeed = owner->GetCharacterMovement()->RotationRate.Yaw;
	}else
	{
		RotationSpeed = InRotationSpeed;
	}
	
	OnFinishedDelegate = OnFinished;
	bIsTurning = true;

	if (!Owner)
	{
		Owner = GetOwner();
	}
	SetComponentTickEnabled(true);
	UE_LOG(LogTemp, Display, TEXT("Rotation Component Started"));
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TurnTimeoutHandle,
			this,
			&URotationComponent::HandleTurnTimeout,
			TurnTimeoutDuration,
			false
		);
	}
}

void URotationComponent::StartSmoothTurnTo(const FVector InTargetLocation, const float InRotationSpeed)
{
	UE_LOG(LogTemp, Display, TEXT("Rotation Component Ticking"));
	TargetLocation = InTargetLocation;
	RotationSpeed = InRotationSpeed;
	
	bIsTurning = true;

	if (!Owner)
	{
		Owner = GetOwner();
	}
	SetComponentTickEnabled(true);
}

void URotationComponent::StartSmoothTurnTo(AActor* NewTargetActor, float InRotationSpeed, FSimpleDelegate OnFinished)
{
	if (bIsTurning)
	{
		OnFinishedDelegate.ExecuteIfBound();
	}
	TargetActor = NewTargetActor;
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	if (InRotationSpeed <= 0.0f)
	{
		RotationSpeed = owner->GetCharacterMovement()->RotationRate.Yaw;
	}else
	{
		RotationSpeed = InRotationSpeed;
	}
	
	OnFinishedDelegate = OnFinished;
	bIsTurning = true;

	if (!Owner)
	{
		Owner = GetOwner();
	}
	SetComponentTickEnabled(true);
	UE_LOG(LogTemp, Display, TEXT("Rotation Component Started"));
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TurnTimeoutHandle,
			this,
			&URotationComponent::HandleTurnTimeout,
			TurnTimeoutDuration,
			false
		);
	}
}

void URotationComponent::StopLockOn()
{
	SetComponentTickEnabled(false);
	bIsLocked = false;
	bIsTurning = false;
	TargetActor = nullptr;
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	if (ABossAIController* controller = Cast<ABossAIController>(owner->GetController()))
	{
		controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
	if (UCharacterMovementComponent* movement = Cast<UCharacterMovementComponent>(owner->GetCharacterMovement()))
	{
		owner->bUseControllerRotationYaw = false;
		movement->bOrientRotationToMovement = true;
		movement->bUseControllerDesiredRotation = false;
	}
}
void URotationComponent::LockOnTarget(AActor* NewTargetActor, float InRotationSpeed)
{
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	if (AAIController* controller = Cast<AAIController>(owner->GetController()))
	{
		controller->SetFocus(NewTargetActor, EAIFocusPriority::Gameplay);
	}
	if (UCharacterMovementComponent* movement = Cast<UCharacterMovementComponent>(owner->GetCharacterMovement()))
	{
		owner->bUseControllerRotationYaw = true;
		movement->bOrientRotationToMovement = false;
		movement->bUseControllerDesiredRotation = true;
	}
	bIsLocked = true;
	bIsTurning = true;

	if (!Owner)
	{
		Owner = GetOwner();
	}
}

void URotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bIsTurning || !Owner) return;

	if (TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}
	
	FRotator Current = Owner->GetActorRotation();
	FRotator Target = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetLocation);
	FRotator NewRot = FMath::RInterpConstantTo(Current, Target, DeltaTime, RotationSpeed);
	Owner->SetActorRotation(NewRot);

	if (FMath::Abs(FRotator::NormalizeAxis(NewRot.Yaw - Target.Yaw)) < 1.0f)
	{
			bIsTurning = false;
			TargetActor = nullptr;
			SetComponentTickEnabled(false);

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TurnTimeoutHandle);
		}
		OnFinishedDelegate.ExecuteIfBound();
	}
}

void URotationComponent::HandleTurnTimeout()
{
	if (bIsTurning)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rotation timed out! Forcing OnFinishedDelegate."));
		bIsTurning = false;
		SetComponentTickEnabled(false);
		OnFinishedDelegate.ExecuteIfBound();
	}
}