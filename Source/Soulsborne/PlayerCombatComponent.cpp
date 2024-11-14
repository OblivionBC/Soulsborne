

#include "PlayerCombatComponent.h"
#include "PlayerEquipment.h"
#include "BaseCharacter.h"
#include "SoulCharacter.h"
#include "DynamicTakeDamage.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameplayEffect.h"
#include "Containers/Map.h"
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
	// We want this to tick since it will handle the TargetLock Logic
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//Check if there is a CameraLockActor, if so change the Player Owners rotation to always face the locked actor
	if (CameraLockActor) {
		if (Owner) {
			APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
			FVector StartLocation = Owner->GetActorLocation(); // Starting point
			FVector TargetLocation = CameraLockActor->GetActorLocation(); // Target point

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
			LookAtRotation.Pitch += -20.0f;
			PlayerController->SetControlRotation(LookAtRotation);
		}
	}
}

void UPlayerCombatComponent::ClearTimer()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(TraceTimerHandle);
}


void UPlayerCombatComponent::DamageTrace()
{
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	//If the cast failed, return
	if (Owner) {

		USkeletalMeshComponent* PlayerMesh = Owner->GetMesh();
		APlayerEquipment* PlayerWeapon = Cast<APlayerEquipment>(Weapon);

		//If the mesh and weapon were retreived successfully, damage trace
		if (PlayerMesh && PlayerWeapon) {

			TArray<FHitResult> HitResults;
			//Query for Pawn Objects
			FCollisionObjectQueryParams NewQueryParams;
			NewQueryParams.AddObjectTypesToQuery(ECC_Pawn);

			//Grab the start and end trace locations for the Player Weapon
			FVector StartLocation = PlayerWeapon->GetStartAttackTrace()->GetComponentLocation();
			FVector EndLocation = PlayerWeapon->GetEndAttackTrace()->GetComponentLocation();
			float HalfHeight = (EndLocation - StartLocation).Size() * 0.5f;
			DamageTraceCollisionParams.AddIgnoredActor(GetOwner());
			float Radius = 35.0f;
			bool bHasHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation, EndLocation,
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeCapsule(Radius, HalfHeight),
				DamageTraceCollisionParams);

			DrawDebugCapsule(GetWorld(), StartLocation + (EndLocation - StartLocation) * 0.5f, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, 2.0f);
			//If we hit something(s), loop through the results and apply logic
			if (bHasHit)
			{
				TMap<AActor*, bool> AttackedActors;
				for (const FHitResult& Result : HitResults)
				{
					ABaseCharacter* HitTarget = Cast<ABaseCharacter>(Result.GetActor());
					//If the HitTarget is an actor, add it to the ignored actors for this DamageTracePeriod and attempt to apply damage

					if (HitTarget && !HitTarget->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll"))))
					{
						if (!AttackedActors.Find(HitTarget)) {
							UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitTarget->GetName());
							HitTarget->printAttributes();
							DamageTraceCollisionParams.AddIgnoredActor(HitTarget);
							AttackedActors.Add(HitTarget, true);
							HitTarget->printAttributes();
							HitTarget->SoulsTakeDamage(PlayerWeapon->maxDamage, PlayerWeapon->DamageType);
						}
					}

				}
				UE_LOG(LogTemp, Warning, TEXT("Hit: %i"), HitResults.Num());
			}

		}
	}
}

//Called By Player Character when the Damage Trace Notify begins
//Handles Initialization for damage tracing
void UPlayerCombatComponent::BeginDamageTrace(AActor* RHandArmament)
{
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	Weapon = Cast<APlayerEquipment>(RHandArmament);
	//If the weapon and Character were retreived successfully, start the Trace logic
	if (Weapon && Owner) {
		DamageTraceCollisionParams.AddIgnoredActor(GetOwner());
		//Creates a Timer for the TracerTimerHandle that plays the DamageTrace function
		Owner->GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &UPlayerCombatComponent::DamageTrace, 0.01f, true);
		DamageTrace();
	}
}

void UPlayerCombatComponent::BeginDamageTrace()
{
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	Weapon = Cast<APlayerEquipment>(Owner->RHandArmament);
	//If the weapon and Character were retreived successfully, start the Trace logic
	if (Weapon && Owner) {
		DamageTraceCollisionParams.AddIgnoredActor(GetOwner());
		//Creates a Timer for the TracerTimerHandle that plays the DamageTrace function
		Owner->GetWorldTimerManager().SetTimer(TraceTimerHandle, this, &UPlayerCombatComponent::DamageTrace, 0.01f, true);
		DamageTrace();
	}
}

//Called by PlayerCharacter when the DamageTrace Notify State ends
void UPlayerCombatComponent::EndDamageTrace()
{
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	if (Owner)
	{
		//Gets rid of the timer and ignored actors
		ClearTimer();
		DamageTraceCollisionParams.ClearIgnoredActors();

	}
}

//Called by the PlayerCharacter when using the bind cor TargetLock
//Traces for a character and binds the CameraLockActor to it, which results in the player camera being locked onto the actor
void UPlayerCombatComponent::TargetLockCamera() {
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	if (Owner) {
		APlayerController* PlayerController = Cast<APlayerController>(Owner->GetController());
		UCharacterMovementComponent* MovementComponent = Owner->GetCharacterMovement();

		//If there is already a CameraLockActor, this is the Unbind Case
		if (CameraLockActor && PlayerController) {
			CameraLockActor = nullptr;
			//Change the Players Movement type to not lock with camera movement
			MovementComponent->bOrientRotationToMovement = true;
			MovementComponent->bUseControllerDesiredRotation = false;
			if (TargetLockIcon) {
				TargetLockIcon->Destroy();
			}

			return;
		}
		//There is no CameraLockActor, meaning this is the Case to find an actor and Bind
		if (PlayerController && !CameraLockActor) {
			APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
			if (CameraManager) {
				//Creates the startlocation as slightly above the character, and traces towards where the camera is facing
				USceneComponent* TransformComponent = CameraManager->GetTransformComponent();
				FVector StartLocation = TransformComponent->GetComponentLocation();
				StartLocation.Z += 50.0;
				FVector ForwardVector = TransformComponent->GetForwardVector();
				ForwardVector *= 1000.0f;
				FVector EndLocation = ForwardVector + StartLocation;
				float SphereRadius = 150.0f;

				//Trace for Pawns
				TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
				ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
				//Ignore Owner in trace
				TArray<AActor*> IgnoreActors;
				IgnoreActors.Add(Owner);
				FHitResult OutHit;

				//Traces in a sphere shape ahead of character to find actors, storing the first found actor in OutHit
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
				//When we hit something and it is a character, bind the CameraLockActor to it and Change the Players Movement
				if (bHit && Cast<ACharacter>(OutHit.GetActor()))
				{
					CameraLockActor = OutHit.GetActor();

					//Lock the Players movement to oriend towards where the camera is (Always Facing the Locked Actor)
					MovementComponent->bOrientRotationToMovement = false;
					MovementComponent->bUseControllerDesiredRotation = true;
					if (TargetLockIconClass)
					{
						//If we have the icon class, spawn it on the targeted actor
						FVector SpawnLocation = FVector(0.0f, 0.0f, 20.0f);
						FRotator SpawnRotation = FRotator::ZeroRotator; //Rotation is not neccessary, as the icon is a UI type so it always faces the user
						FActorSpawnParameters SpawnParams;
						//Spawn Actor and attatch it to the character
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


}

void UPlayerCombatComponent::TakeDamage()
{
	if (!Owner) {
		Owner = Cast<ABaseCharacter>(GetOwner());
	}
	if (Owner) {
		//Owner->PlayAnimMontage(TakeDamageMontage);
		//Owner->SoulsTakeDamage();
		double health;
		Owner->GetHealth_Implementation(health);
		if (health <= 0) {
			Owner->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.isDead")));
			Owner->GetMesh()->SetSimulatePhysics(true);
			Owner->GetMesh()->GetAnimInstance()->Montage_Stop(0.0f);
		}
	}
}

//Attempts to apply damage to a target actor (Is currently static value)
void UPlayerCombatComponent::ApplyDamage(AActor* Target, float Damage) {
	if (Target) {
		UAbilitySystemComponent* TargetASC = Target->FindComponentByClass<UAbilitySystemComponent>();
		//if the target has a Ability System Component, try to apply Damage Gameplay Effect to it
		if (TargetASC) {

			//Create a spec for the DamageEffectClass
			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(UDynamicTakeDamage::StaticClass(), 1.0f, TargetASC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get();
				if (EffectSpec)
				{
					FGameplayModifierInfo DamageModifier;
					DamageModifier.ModifierOp = EGameplayModOp::Additive;
					DamageModifier.ModifierMagnitude = FScalableFloat(Damage * -1);
					TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec);
				}

				//UE_LOG(LogTemp, Warning, TEXT("Target Health After %f"), Target->GetAbilitySystemComponent()->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
			}
		}
	}
}
