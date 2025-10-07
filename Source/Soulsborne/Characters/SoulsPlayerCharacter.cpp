// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsPlayerCharacter.h"

#include "../Abilities/SoulAttributeSet.h"
#include "../Abilities/Dodge.h"
#include "../Abilities/AttackCombo.h"

#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagsModule.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../GameplayTags/SoulsGameplayTags.h"
#include "Components/ProgressBar.h"
#include "../Items/WeaponItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Soulsborne/UI/PlayerHUDWidget.h"


///////////////////////////////////////////////////             Base Functions                ///////////////////////////////////////////////////////////////

ASoulsPlayerCharacter::ASoulsPlayerCharacter()
{
	//Initialize Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create the follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	EquippedItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedItemMesh"));
	EquippedItem->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquippedItem->SetCollisionResponseToAllChannels(ECR_Ignore);
	EquippedItem->SetupAttachment(GetMesh(), TEXT("hand_r"));
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	SetupStimulusSource();
	PrimaryActorTick.bCanEverTick = true;
}

/** Called when the game starts or when spawned */
void ASoulsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	StartStaminaRegen();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		if (HUDWidget)
		{
			PlayerHUD = CreateWidget<UPlayerHUDWidget>(PlayerController, HUDWidget, "HUD");
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
				PlayerHUD->UpdateHealthBar(MaxHealth, MaxHealth);
				PlayerHUD->UpdateStaminaBar(MaxStamina, MaxStamina);
			}
		}
		//Static call for now, planning to implement based on a save file

		AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Identity_Player);
		AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Class_Knight);
		bIsInvulnerable = false;
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MyInputMappingContext, 1);
		}
	}
}

/** Called every frame */
void ASoulsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTargetLock();
}

/** Called when the character is possessed by a controller */
void ASoulsPlayerCharacter::PossessedBy(AController* NewController)
{
	// Call the base class version
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
	PrintAttributes();
}

/** Called when the player state is replicated */
void ASoulsPlayerCharacter::OnRep_PlayerState()
{
	// Call the base class version
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
	PrintAttributes();
}

///////////////////////////////////////////////////             Attribute Functions                ///////////////////////////////////////////////////////////

bool ASoulsPlayerCharacter::IsHoldingWeapon()
{
	UWeaponItem* WeaponItem = Cast<UWeaponItem>(InventorySlots[EquippedItemIndex]);
	if (WeaponItem) return true;
	return false;
}

/* Initialize the character's attributes */
void ASoulsPlayerCharacter::InitializeAttributes()
{
	ApplyGameplayEffectToSelf(StartingStatEffect);
	ApplyGameplayEffectToSelf(RechargeStaminaEffect);
}

void ASoulsPlayerCharacter::StartDamageTrace_Implementation() const
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 50.f;
	FVector End = Start + GetActorForwardVector() * 100.0f;

	FCollisionShape Capsule = FCollisionShape::MakeCapsule(34.f, 88.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> HitResults;

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		Capsule,
		QueryParams
	);
	
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			ABaseCharacter* Character = Cast<ABaseCharacter>(HitActor);
			if (!Character || Character == this) continue;

			float Damage;
			EDamageType DamageType;
			if (UWeaponItem* Weapon = Cast<UWeaponItem>(InventorySlots[EquippedItemIndex]))
			{
				Damage = Weapon->AttackPower;
				DamageType = Weapon->DamageType;
			} else
			{
				Damage = 25;
				DamageType = EDamageType::Blunt;
			}
			Character->SoulsTakeDamage(Damage, DamageType);
		}
	}
}

void ASoulsPlayerCharacter::EndDamageTrace_Implementation() const
{
	// Clean up any ongoing trace effects if needed
}

void ASoulsPlayerCharacter::SoulsTakeDamage(float DamageAmount, EDamageType DamageType)
{
	UE_LOG(LogTemp, Warning, TEXT("HERE IS THE DAMAGE AMOUNT %f"), DamageAmount);
	Super::SoulsTakeDamage(DamageAmount, DamageType);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	bool bIsBlocking = false;
	if (bIsInvulnerable) return;
	if (ASC)
	{
		const FGameplayTagContainer& CurrTags = ASC->GetOwnedGameplayTags();
		for (const FGameplayTag& Tag : CurrTags)
		{
			if (Tag == FGameplayTag::RequestGameplayTag(FName("State.IsBlocking")))
			{
				DamageAmount = DamageAmount * .6;
				bIsBlocking = true;
				break;
			}
		}
		FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		float CurrentHealth = ASC->GetNumericAttribute(HealthAttribute);
		float NewHealth = CurrentHealth - DamageAmount;
		ASC->SetNumericAttributeBase(HealthAttribute, NewHealth);
		CurrentHealth = ASC->GetNumericAttribute(HealthAttribute);
		if (PlayerHUD)
		{
			PlayerHUD->UpdateHealthBar(CurrentHealth, MaxHealth);
		}
		if (CurrentHealth <= 0 && !bIsDead)
		{
			Die();
		}
		else
		{
			if (HitMontage)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_Play(HitMontage);
				}
			}
		}
	}
}

void ASoulsPlayerCharacter::SoulsHeal(float HealAmount)
{
	Super::SoulsHeal(HealAmount);
	double CurrentHealth = 0;
	GetHealth_Implementation(CurrentHealth);
	if (PlayerHUD)
	{
		PlayerHUD->UpdateHealthBar(CurrentHealth, MaxHealth);
	}
}

void ASoulsPlayerCharacter::Die()
{
	Super::Die();
	UE_LOG(LogTemp, Warning, TEXT("Player Died"));
	OnPlayerKilled.Broadcast(this);
	this->GetMesh()->SetSimulatePhysics(true);
	this->GetCharacterMovement()->DisableMovement();
	this->StimulusSource->UnregisterFromPerceptionSystem();
	this->StimulusSource->bAutoRegister = false;
	bIsDead = true;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.isDead")));
	}
}

void ASoulsPlayerCharacter::StartStaminaRegen()
{
	if (!GetWorld()) return;

	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimer))
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		StaminaRegenTimer,
		this,
		&ASoulsPlayerCharacter::RegenStamina,
		StaminaRegenInterval,
		true
	);
}

void ASoulsPlayerCharacter::StopStaminaRegen()
{
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
}

void ASoulsPlayerCharacter::RegenStamina()
{
	float CurrentStamina = GetAbilitySystemComponent()->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute());
	float NewStamina = CurrentStamina + StaminaRegenRate;

	if (NewStamina >= MaxStamina)
	{
		NewStamina = MaxStamina;
		StopStaminaRegen();
	}
	FGameplayAttribute StamAttribute = USoulAttributeSet::GetStaminaAttribute();
	GetAbilitySystemComponent()->SetNumericAttributeBase(StamAttribute, NewStamina);
	if (PlayerHUD) PlayerHUD->UpdateStaminaBar(NewStamina, MaxStamina);
}

bool ASoulsPlayerCharacter::UseStamina(const float StaminaAmnt)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	float CurrentStamina = ASC->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute());
	if (CurrentStamina < StaminaAmnt) return false;

	FGameplayAttribute StamAttribute = USoulAttributeSet::GetStaminaAttribute();
	float NewStam = CurrentStamina - StaminaAmnt;
	ASC->SetNumericAttributeBase(StamAttribute, NewStam);
	if (PlayerHUD) PlayerHUD->UpdateStaminaBar(NewStam, MaxStamina);
	return true;
}


///////////////////////////////////////////////////             Abilities                ///////////////////////////////////////////////////////////////////
//Apply a gameplay effect to player
void ASoulsPlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply)
{
	if (AbilitySystemComponent && EffectToApply)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		//EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			EffectToApply, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get());
		}
	}
}

/** Give the character default abilities */
void ASoulsPlayerCharacter::GiveDefaultAbilities()
{
	// Grant abilities, but only on the server
	if (HasAuthority() && AbilitySystemComponent)
	{
		// Give the default abilities, using the default level of 1
		for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, 0));
		}
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackComboAbility, 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DodgeAbility, 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DrinkAbility, 1, 0));
	}
}

///////////////////////////////////////////////////             Inputs                //////////////////////////////////////////////////////////////////////

void ASoulsPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputAction* IA_MoveForward = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr,
	                                                                   TEXT(
		                                                                   "/Game/Soulsbourne/Input/IA_MoveForward.IA_MoveForward")));
	UInputAction* IA_MoveRight = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr,
	                                                                 TEXT(
		                                                                 "/Game/Soulsbourne/Input/IA_MoveRight.IA_MoveRight")));
	UInputAction* IA_Block = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Block.IA_Block")));
	UInputAction* IA_Jump = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Jump.IA_Jump")));
	UInputAction* IA_LookRight = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr,
	                                                                 TEXT(
		                                                                 "/Game/Soulsbourne/Input/IA_LookRight.IA_LookRight")));
	UInputAction* IA_LookUp = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_LookUp.IA_LookUp")));
	UInputAction* IA_Roll = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Roll.IA_Roll")));
	UInputAction* IA_LockCamera = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr,
	                                                                  TEXT(
		                                                                  "/Game/Soulsbourne/Input/IA_LockCamera.IA_LockCamera")));
	UInputAction* IA_Drink = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Heal.IA_Heal")));
	UInputAction* IA_SwapItem = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_SwapItem.IA_SwapItem")));
	UInputAction* IA_Attack = Cast<UInputAction>(
		StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Attack.IA_Attack")));
	


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Basic Move and Look
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this,
		                                   &ASoulsPlayerCharacter::MoveRight, IA_MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this,
		                                   &ASoulsPlayerCharacter::MoveForward, IA_MoveForward);

		EnhancedInputComponent->BindAction(IA_LookRight, ETriggerEvent::Triggered, this,
		                                   &ASoulsPlayerCharacter::LookRight);
		EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::LookUp);
		//Abilities
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::PlayerJump);
		EnhancedInputComponent->BindAction(IA_LockCamera, ETriggerEvent::Started, this,
		                                   &ASoulsPlayerCharacter::LockCamera);

		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Ongoing, this, &ASoulsPlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Completed, this,
		                                   &ASoulsPlayerCharacter::BlockComplete);

		EnhancedInputComponent->BindAction(IA_Drink, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::Drink);
		EnhancedInputComponent->BindAction(IA_SwapItem, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::SwapItem);
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::Dodge);
		
	}
}

void ASoulsPlayerCharacter::LockCamera(const FInputActionValue& Value)
{
	TargetLockCamera();
}

void ASoulsPlayerCharacter::UpdateTargetLock()
{
	if (CameraLockActor)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			const FVector StartLocation = GetActorLocation();
			const FVector TargetLocation = CameraLockActor->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
			LookAtRotation.Pitch += -20.0f;
			PlayerController->SetControlRotation(LookAtRotation);
		}
	}
}

void ASoulsPlayerCharacter::TargetLockCamera()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (CameraLockActor && PlayerController)
	{
		CameraLockActor = nullptr;
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		if (TargetLockIcon)
		{
			TargetLockIcon->Destroy();
		}
		return;
	}

	if (PlayerController && !CameraLockActor)
	{
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			USceneComponent* TransformComponent = CameraManager->GetTransformComponent();
			FVector StartLocation = TransformComponent->GetComponentLocation();
			StartLocation.Z += 50.0;
			FVector ForwardVector = TransformComponent->GetForwardVector() * 1500.0f;
			const FVector EndLocation = ForwardVector + StartLocation;
			const float SphereRadius = 200.0f;

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
			ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> IgnoreActors;
			IgnoreActors.Add(this);
			FHitResult OutHit;

			const bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(),
				StartLocation,
				EndLocation,
				SphereRadius,
				ObjectTypesArray,
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				OutHit,
				true);

			if (bHit && Cast<ACharacter>(OutHit.GetActor()))
			{
				CameraLockActor = OutHit.GetActor();
				MovementComponent->bOrientRotationToMovement = false;
				MovementComponent->bUseControllerDesiredRotation = true;

				if (TargetLockIconClass)
				{
					const FVector SpawnLocation = FVector(0.0f, 0.0f, 20.0f);
					const FRotator SpawnRotation = FRotator::ZeroRotator;
					FActorSpawnParameters SpawnParams;
					TargetLockIcon = GetWorld()->SpawnActor<AActor>(TargetLockIconClass, SpawnLocation, SpawnRotation, SpawnParams);
					if (TargetLockIcon)
					{
						const FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
						TargetLockIcon->AttachToActor(CameraLockActor, AttachmentRules);

						FTimerHandle DestroyTimerHandle;
						GetWorldTimerManager().SetTimer(
							DestroyTimerHandle,
							[this]()
							{
								if (TargetLockIcon && IsValid(TargetLockIcon))
								{
									TargetLockIcon->Destroy();
									TargetLockIcon = nullptr;
								}
							},
							2.0f,
							false
						);
					}
				}
			}
		}
	}
}

void ASoulsPlayerCharacter::Pickup_Implementation(TSubclassOf<UItem> Item)
{
	if (ItemsPicked < 4)
	{
		if (UItem* ItemInstance = NewObject<UItem>(this, Item))
		{
			for (int i = 0; i < ItemsPicked; i++)
			{
				if (InventorySlots[i] && InventorySlots[i]->ItemName == ItemInstance->ItemName)
				{
					InventorySlots[i]->Number++;
					CheckItemCategory(ItemInstance, i);
					return;
				}
			}
			ItemInstance->Number = 1;
			InventorySlots[ItemsPicked] = ItemInstance;
			CheckItemCategory(ItemInstance, ItemsPicked);
			ItemsPicked++;
		}
	}
}

void ASoulsPlayerCharacter::CheckItemCategory(UItem* Item, int slot)
{
	if (Item->ItemCategory == "Weapons")
	{
		PlayerHUD->GetInventoryWidget()->SetSlot(
			slot,
			Item->Icon);
	}
	else
	{
		PlayerHUD->GetInventoryWidget()->SetSlot(
			slot,
			Item->Icon,
			InventorySlots[slot]->Number
		);
	}
	if (slot == EquippedItemIndex)
	{
		EquippedItem->SetStaticMesh(InventorySlots[EquippedItemIndex]->ItemMesh);
	}
}

void ASoulsPlayerCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

void ASoulsPlayerCharacter::EquipItem(TSubclassOf<UItem> Item)
{
	if (UItem* ItemInstance = NewObject<UItem>(this, Item))
	{
		EquippedItem->SetStaticMesh(ItemInstance->ItemMesh);
		EquippedItem->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


//Blueprint for now, will be migrated to Gameplay Ability Soon
void ASoulsPlayerCharacter::BlockComplete(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
		FGameplayTagContainer BlockTagContainer;
		BlockTagContainer.AddTag(BlockTag);
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(
			BlockTagContainer);
	}
}

void ASoulsPlayerCharacter::Drink(const FInputActionValue& Value)
{
	if (DrinkAbility)
	{
		int PotionIndex = 0;
		UItem* PotionItem = nullptr;
		for (int i = 0; i < ItemsPicked; i++)
		{
			if (InventorySlots[i] && InventorySlots[i]->ItemName == TEXT("Potion"))
			{
				PotionItem = InventorySlots[i];
				PotionIndex = i;
			}
		}
		if (!PotionItem) return;
		
		PotionItem->Number--;
		if (PotionItem->Number == 0)
		{
			InventorySlots[PotionIndex] = nullptr;
			PlayerHUD->GetInventoryWidget()->SetSlot(PotionIndex, nullptr, 0);
		}
		EquippedItem->SetStaticMesh(PotionItem->ItemMesh);
		AbilitySystemComponent->TryActivateAbilityByClass(DrinkAbility);
	}
}

void ASoulsPlayerCharacter::SwapItem(const FInputActionValue& Value)
{
	EquippedItemIndex++;
	if (EquippedItemIndex >= InventorySlots.Num()) EquippedItemIndex = 0;
	PlayerHUD->GetInventoryWidget()->SetEquipped(EquippedItemIndex);
	if (InventorySlots[EquippedItemIndex])
	{
		EquippedItem->SetStaticMesh(InventorySlots[EquippedItemIndex]->ItemMesh);
	}else
	{
		EquippedItem->SetStaticMesh(nullptr);
	}
}

void ASoulsPlayerCharacter::Dodge(const FInputActionValue& Value)
{
	AbilitySystemComponent->TryActivateAbilityByClass(DodgeAbility);
}

void ASoulsPlayerCharacter::Block(const FInputActionValue& Value)
{
	FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
	FGameplayTagContainer BlockTagContainer;
	BlockTagContainer.AddTag(BlockTag);
	AbilitySystemComponent->TryActivateAbilitiesByTag(BlockTagContainer);
}

//Blueprint for now, will be migrated to Gameplay Ability Soon
void ASoulsPlayerCharacter::PlayerJump(const FInputActionValue& Value)
{
	double CurrentStamina;
	ABaseCharacter::GetStamina_Implementation(CurrentStamina);
	if (CurrentStamina > 15.0f)
	{
		FGameplayTag JumpTag = UGameplayTagsManager::Get().RequestGameplayTag("Player.Abilities.Jump");
		FGameplayTagContainer JumpTagContainer;
		JumpTagContainer.AddTag(JumpTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(JumpTagContainer);
		//ApplyGameplayEffectToSelf(UseStamina);
	}
}

void ASoulsPlayerCharacter::MoveForward(const FInputActionValue& Value, UInputAction* Action)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		// Get the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//Make it slightly slower if going backwards
		if (Value.Get<float>() < 0.0f)
		{
			AddMovementInput(Direction, (Value.Get<float>() / 1.4));
		}
		else
		{
			AddMovementInput(Direction, Value.Get<float>());
		}
		DirectionKey = ABaseCharacter::GetMovementDirection(Action);
	}
}

void ASoulsPlayerCharacter::MoveRight(const FInputActionValue& Value, UInputAction* Action)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		// Get the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value.Get<float>());
		DirectionKey = GetMovementDirection(Action);
	}
}

void ASoulsPlayerCharacter::LookRight(const FInputActionValue& Value)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		AddControllerYawInput(Value.Get<float>());
	}
}

void ASoulsPlayerCharacter::LookUp(const FInputActionValue& Value)
{
	if (Controller && Value.Get<float>() != 0.0f)
	{
		AddControllerPitchInput(Value.Get<float>());
	}
}
