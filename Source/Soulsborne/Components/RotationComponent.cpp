// BossRotationComponent.cpp

#include "Components.h"
#include "RotationComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

URotationComponent::URotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URotationComponent::BeginPlay()
{
	Super::BeginPlay();
	RegisterComponent();
	PrimaryComponentTick.RegisterTickFunction(GetComponentLevel());
	Owner = GetOwner();
	SetComponentTickEnabled(false);
}

void URotationComponent::StartSmoothTurnTo(FVector InTargetLocation, float InRotationSpeed, FSimpleDelegate OnFinished)
{
	TargetLocation = InTargetLocation;
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	if (owner)
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
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void URotationComponent::StartSmoothTurnTo(FVector InTargetLocation, float InRotationSpeed)
{
	TargetLocation = InTargetLocation;
	ACharacter * owner = Cast<ACharacter>(GetOwner());
	RotationSpeed = InRotationSpeed;
	
	bIsTurning = true;

	if (!Owner)
	{
		Owner = GetOwner();
	}
	PrimaryComponentTick.SetTickFunctionEnable(true);
	SetComponentTickEnabled(true);
}

void URotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bIsTurning || !Owner) return;

	FRotator Current = Owner->GetActorRotation();
	FRotator Target = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), TargetLocation);
	FRotator NewRot = FMath::RInterpConstantTo(Current, Target, DeltaTime, RotationSpeed);

	Owner->SetActorRotation(NewRot);

	if (FMath::Abs(FRotator::NormalizeAxis(NewRot.Yaw - Target.Yaw)) < 1.0f)
	{
		bIsTurning = false;
		SetComponentTickEnabled(false);
		OnFinishedDelegate.ExecuteIfBound();
	}
}
