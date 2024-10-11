// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"
#include "PlayerEquipment.h"
#include "SoulCharacter.h"
#include "DynamicTakeDamage.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "UObject/UnrealType.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPlayerCombatComponent::UPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());
}


// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CameraLockActor) {
		ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		FVector StartLocation = MyOwner->GetActorLocation(); // Starting point
		FVector TargetLocation = CameraLockActor->GetActorLocation(); // Target point

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		LookAtRotation.Pitch += -20.0f;
		PlayerController->SetControlRotation(LookAtRotation);
	}
	// ...
}


void UPlayerCombatComponent::DamageTrace()
{
	// Implement sphere trace logic here
	ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
	if (!MyOwner) {
		return;
	}
	USkeletalMeshComponent* PlayerMesh = MyOwner->GetMesh();

	APlayerEquipment* PlayerWeapon = Cast<APlayerEquipment>(Weapon);
	if (PlayerMesh && PlayerWeapon) {
		TArray<FHitResult> HitResults;
		FCollisionObjectQueryParams NewQueryParams;
		NewQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector StartLocation = PlayerWeapon->GetStartAttackTrace()->GetComponentLocation();
		FVector EndLocation = PlayerWeapon->GetEndAttackTrace()->GetComponentLocation();
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(MyOwner);
		bool bHasHit = GetWorld()->LineTraceMultiByObjectType(
			HitResults,
			StartLocation,
			EndLocation,
			NewQueryParams,
			CollisionParams);

		//DrawDebugSphere(GetWorld(), RightSocketLocation, SphereRadius, 12, FColor::Green, false, 5.0f);
		DrawDebugLine(
			GetWorld(),
			StartLocation,
			EndLocation,
			FColor::Green,
			false, 1, 0, 1
		);
		if (bHasHit)
		{
			for (const FHitResult& Result : HitResults)
			{
				AActor* HitTarget = Result.GetActor();
				if (HitTarget)
				{
					// Debug message to check each overlapped actor

					//Deal Damage
					ApplyDamage(HitTarget, 40.0f);
					//DamageDealt.Broadcast(this, 20, this->GetController());

				}

			}
		}

	}
}

void UPlayerCombatComponent::BeginDamageTrace(AActor* RHandArmament)
{
	ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
	Weapon = Cast<APlayerEquipment>(RHandArmament);
	if (Weapon && MyOwner) {
		MyOwner->GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &UPlayerCombatComponent::DamageTrace, 0.01f, true);
		DamageTrace();
	}
}

void UPlayerCombatComponent::EndDamageTrace()
{
	if (Owner)
	{
		Owner->GetWorldTimerManager().ClearTimer(TraceTimerHandle);
	}
}

void UPlayerCombatComponent::TargetLockCamera() {
	ACharacter* MyOwner = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
	UCharacterMovementComponent* MovementComponent = MyOwner->GetCharacterMovement();
	if (CameraLockActor && PlayerController) {
		CameraLockActor = nullptr;
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		if (TargetLockIcon) {
			TargetLockIcon->Destroy();
		}

		return;
	}
	if (PlayerController) {
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager) {
			USceneComponent* TransformComponent = CameraManager->GetTransformComponent();
			FVector StartLocation = TransformComponent->GetComponentLocation();
			StartLocation.Z += 50.0;
			FVector ForwardVector = TransformComponent->GetForwardVector();
			ForwardVector *= 1000.0f;
			FVector EndLocation = ForwardVector + StartLocation;
			float SphereRadius = 150.0f;

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
			ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(MyOwner);
			FHitResult OutHit;
			bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				StartLocation,
				EndLocation,
				SphereRadius,
				ObjectTypesArray,
				false,
				IgnoreActors,
				EDrawDebugTrace::ForDuration,
				OutHit,
				true
			);
			if (bHit && Cast<ACharacter>(OutHit.GetActor()))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OutHit.Actor->GetName());
				CameraLockActor = OutHit.GetActor();
				MovementComponent->bOrientRotationToMovement = false;
				MovementComponent->bUseControllerDesiredRotation = true;
				if (TargetLockIconClass)
				{
					FVector SpawnLocation = FVector(0.0f, 0.0f, 20.0f); // Example location
					FRotator SpawnRotation = FRotator::ZeroRotator; // Example rotation
					FActorSpawnParameters SpawnParams;
					TargetLockIcon = GetWorld()->SpawnActor<AActor>(TargetLockIconClass, SpawnLocation, SpawnRotation, SpawnParams);
					if (TargetLockIcon)
					{
						FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
						TargetLockIcon->AttachToActor(CameraLockActor, AttachmentRules);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No Hit"));
			}
		}
	}
}

void UPlayerCombatComponent::ApplyDamage(AActor* Target, float Damage) {
	Damage *= -1.0f;
	if (Target) {
		UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>();
		if (TargetASC) {
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(UDynamicTakeDamage::StaticClass(), 1.0f, TargetASC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

				//Caller Magnitude is not working dynamically, hard coded in Effect
				Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
				TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);

				ASoulCharacter* SoulCharacter = Cast<ASoulCharacter>(Target);
				if (SoulCharacter) {
					SoulCharacter->Damaged();
				}
				//UE_LOG(LogTemp, Warning, TEXT("Target Health After %f"), Target->GetAbilitySystemComponent()->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
			}
		}
	}

}