

#include "PlayerCharacter.h"	
#include "SoulAttributeSet.h"	
#include "CharacterAttackCombo.h"
#include "DynamicTakeDamage.h"
#include "PlayerCombatInterface.h"
#include "PlayerEquipment.h"
#include "ProgressBarInterface.h"

#include "AbilitySystemComponent.h"	
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagsModule.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


/*** ----------------------- Base Functions ---------------------- ***/

/** Sets default values */
APlayerCharacter::APlayerCharacter()
{
	// Initialize the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Initialize the attribute set
	Attributes = CreateDefaultSubobject<USoulAttributeSet>("AttributesSet");

	//Initialize Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create the follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Montage(TEXT("/Game/Soulsbourne/Animations/Gorka/Combat/MONT_SwordAttackCombo.MONT_SwordAttackCombo"));
	if (Montage.Succeeded())
	{
		MontageToPlay = Montage.Object;
	}
}

/** Called when the game starts or when spawned */
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		if (HUDWidget)
		{
			PlayerHUD = CreateWidget<UUserWidget>(PlayerController, HUDWidget, "HUD");
			check(PlayerHUD);
			PlayerHUD->AddToPlayerScreen();

		}
		//Static call for now
		FName rightSocketName = "righthandSocket";
		AttatchEquipment(LHandArmamentClass, rightSocketName);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MyInputMappingContext, 1);
		}

		this->GetMesh()->GetAnimInstance()->OnPlayMontageNotifyEnd.AddDynamic(this, &APlayerCharacter::OnMontageNotifyStart);
	}
}
void APlayerCharacter::OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) {
	UE_LOG(LogTemp, Log, TEXT("isAttacking : %s"), (isAttacking ? TEXT("true") : TEXT("false")));
	UE_LOG(LogTemp, Log, TEXT("isComboActive : %s"), (isComboActive ? TEXT("true") : TEXT("false")));
	OnMontageNotify.Broadcast(NotifyName, BranchingPointNotifyPayload);
	if (MontageToPlay && !isAttacking)
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, MontageToPlay);
		isComboActive = false;
		isAttacking = false;
		return;
	}
	isAttacking = false;
	HandleMeleeAttack();
}
/** Called every frame */
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CameraLockActor) {

		FVector StartLocation = this->GetActorLocation(); // Starting point
		FVector TargetLocation = CameraLockActor->GetActorLocation(); // Target point

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		LookAtRotation.Pitch += -20.0f;
		this->GetController()->SetControlRotation(LookAtRotation);
	}
}

/** Called when the character is possessed by a controller */
void APlayerCharacter::PossessedBy(AController* NewController)
{
	// Call the base class version
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
}

/** Called when the player state is replicated */
void APlayerCharacter::OnRep_PlayerState()
{
	// Call the base class version
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
}

/*** ----------------------- Attribute Functions ---------------------- ***/

/** Initialize the character's attributes */
void APlayerCharacter::InitializeAttributes()
{
	ApplyGameplayEffectToSelf(StartingStatEffect);
	ApplyGameplayEffectToSelf(RechargeStaminaEffect);
}

/** Player Progress Bar Interface Implementations **/
void APlayerCharacter::GetStamina_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void APlayerCharacter::GetMana_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetManaAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void APlayerCharacter::GetHealth_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void APlayerCharacter::GetStaminaAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute()) / MaxStamina;
	}
	else {
		Result = 0.0f;
	}
}
void APlayerCharacter::GetManaAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetManaAttribute()) / MaxMana;
	}
	else {
		Result = 0.0f;
	}
}
void APlayerCharacter::GetHealthAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()) / MaxHealth;
	}
	else {
		Result = 0.0f;
	}
}

/** Combat Interface Implementations **/
void APlayerCharacter::StartDamageTrace_Implementation() const
{
	if (PlayerCombatComponent)
	{
		PlayerCombatComponent->BeginDamageTrace();
	}
}

void APlayerCharacter::EndDamageTrace_Implementation() const
{
	if (PlayerCombatComponent)
	{
		PlayerCombatComponent->EndDamageTrace();
	}
}
/*** --------------------------------- Abilities ------------------------------- ***/

void APlayerCharacter::HandleMeleeAttack() {
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh && RHandArmament) {
		FName rightSocketName = "righthandSocket";
		FVector RightSocketLocation = PlayerMesh->GetSocketLocation(rightSocketName);
		TArray<FHitResult> HitResults;
		FCollisionObjectQueryParams NewQueryParams;
		NewQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		APlayerEquipment* Weapon = Cast<APlayerEquipment>(RHandArmament);
		if (!Weapon) {
			return;
		}
		FVector StartLocation = Weapon->GetStartAttackTrace()->GetComponentLocation();
		FVector EndLocation = Weapon->GetEndAttackTrace()->GetComponentLocation();
		EndLocation.X += 100.0f;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		//ArrowComponent* StartAttackTrace = Cast<UArrowComponent>(RightHandArmament->GetComponentByName(UArrowComponent::StaticClass(), TEXT("StartAttackTrace")));
		//ArrowComponent* EndAttackTrace = Cast<UArrowComponent>(RightHandArmament->GetComponentByName(UArrowComponent::StaticClass(), TEXT("EndAttackTrace")));

		if (1 == 1) {
			bool bHasOverlapped = GetWorld()->LineTraceMultiByObjectType(
				HitResults,
				StartLocation,
				EndLocation,
				NewQueryParams,
				CollisionParams);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Overlap check called"));
			}
			//DrawDebugSphere(GetWorld(), RightSocketLocation, SphereRadius, 12, FColor::Green, false, 5.0f);
			DrawDebugLine(
				GetWorld(),
				StartLocation,
				EndLocation,
				FColor::Green,
				false, 1, 0, 1
			);
			if (bHasOverlapped)
			{
				for (const FHitResult& Result : HitResults)
				{
					ASoulCharacter* OverlappedActor = Cast<ASoulCharacter>(Result.GetActor());
					if (OverlappedActor)
					{
						// Debug message to check each overlapped actor
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Overlapped with: %s"), *OverlappedActor->GetName()));
						}

						//Deal Damage
						ApplyDamage(OverlappedActor, 40.0f);
						//DamageDealt.Broadcast(this, 20, this->GetController());

					}

				}
			}
		}
	}

}

void APlayerCharacter::ApplyDamage(ASoulCharacter* Target, float Damage) {
	Damage *= -1.0f;
	//UE_LOG(LogTemp, Warning, TEXT("Target Health Before %f"), Target->GetAbilitySystemComponent()->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UDynamicTakeDamage::StaticClass(), 1.0f, AbilitySystemComponent->MakeEffectContext());
	if (SpecHandle.IsValid() && Target->GetAbilitySystemComponent())
	{
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

		//Caller Magnitude is not working dynamically, hard coded in Effect
		Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Damage);
		Target->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Applied Affect to with: %s"), *Target->GetName()));
		}
		Target->Damaged();
		//UE_LOG(LogTemp, Warning, TEXT("Target Health After %f"), Target->GetAbilitySystemComponent()->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
	}
}

//Apply a gameplay effect to player
void APlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply) {
	if (AbilitySystemComponent && EffectToApply) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		//EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Effect Applied"));
			}
		}
	}
}

/** Override to return the ability system component */
UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

/** Give the character default abilities */
void APlayerCharacter::GiveDefaultAbilities()
{
	// Grant abilities, but only on the server
	if (HasAuthority() && AbilitySystemComponent)
	{
		// Give the default abilities, using the default level of 1
		for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, 0));
		}
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UCharacterAttackCombo::StaticClass(), 1, 0));
	}
}

//Used to attatch equipment to the socket name specified, currently static for development purposes
void APlayerCharacter::AttatchEquipment(TSubclassOf<AActor> Equipment, FName socketName) {
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh && LHandArmamentClass && RHandArmamentClass) {
		FName rightSocketName = "righthandSocket";
		FName leftSocketName = "lefthandSocket";
		FTransform LeftSocketT = PlayerMesh->GetSocketTransform(leftSocketName);
		FTransform RightSocketT = PlayerMesh->GetSocketTransform(rightSocketName);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ATTACHING")));
		FActorSpawnParameters SpawnParams;
		LHandArmament = GetWorld()->SpawnActor<AActor>(LHandArmamentClass, LeftSocketT, SpawnParams);
		RHandArmament = GetWorld()->SpawnActor<AActor>(RHandArmamentClass, RightSocketT, SpawnParams);

		LHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, leftSocketName);
		RHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, rightSocketName);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("APPLIED"));
		}

	}


}

//////////////////////////////////////////////////////////////////////////////////             Inputs                /////////////////////////////////////////////////////////////////////////////////////////////////

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UInputAction* IA_MoveForward = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_MoveForward.IA_MoveForward")));
	UInputAction* IA_Attack = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Attack.IA_Attack")));
	UInputAction* IA_MoveRight = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_MoveRight.IA_MoveRight")));
	UInputAction* IA_Block = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Block.IA_Block")));
	UInputAction* IA_Jump = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Jump.IA_Jump")));
	UInputAction* IA_LookRight = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_LookRight.IA_LookRight")));
	UInputAction* IA_LookUp = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_LookUp.IA_LookUp")));
	UInputAction* IA_Roll = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_Roll.IA_Roll")));
	UInputAction* IA_LockCamera = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("/Game/Soulsbourne/Input/IA_LockCamera.IA_LockCamera")));


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Basic Move and Look
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward);
		EnhancedInputComponent->BindAction(IA_LookRight, ETriggerEvent::Triggered, this, &APlayerCharacter::LookRight);
		EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);

		//Ability Movement
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerJump);
		EnhancedInputComponent->BindAction(IA_LockCamera, ETriggerEvent::Started, this, &APlayerCharacter::LockCamera);
		//Combat
		EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::AttackComboNoDelegates);

		//EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::AttackCombo);
		//EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);

		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Triggered, this, &APlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Ongoing, this, &APlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Completed, this, &APlayerCharacter::BlockComplete);

		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &APlayerCharacter::Roll);

	}
}

void APlayerCharacter::LockCamera(const FInputActionValue& Value) {
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UCharacterMovementComponent* MovementComponent = this->GetCharacterMovement();
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
			IgnoreActors.Add(this);
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
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Spawned"));
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

void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ATTACK"));

	double stam;
	GetStamina_Implementation(stam);

	if (stam > 15.0f) {
		FGameplayTag AttackTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.Attack");
		FGameplayTagContainer AttackTagContainer;
		AttackTagContainer.AddTag(AttackTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(AttackTagContainer);
	}

}

void APlayerCharacter::AttackCombo(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ATTACK"));
	isAttacking = true;
	double stam;
	GetStamina_Implementation(stam);
	FName ComboTag = "Character.AttackCombo";
	if (stam > 15.0f && AbilitySystemComponent->ComponentHasTag(ComboTag)) {
		isAttacking = true;
		//Combo is on still, on the Input, check if the player is still comboing

		//FGameplayTag AttackComboTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.AttackCombo");
		//FGameplayTagContainer AttackComboTagContainer;
		//AttackComboTagContainer.AddTag(AttackComboTag);
		//UE_LOG(LogTemp, Warning, TEXT("Activate %s"), (AbilitySystemComponent->TryActivateAbilitiesByTag(AttackComboTagContainer) ? TEXT("true") : TEXT("false")));
		//AbilitySystemComponent->TryActivateAbilitiesByTag(AttackComboTagContainer);
	}
	else if (stam > 15.0f) {
		AbilitySystemComponent->TryActivateAbilityByClass(UCharacterAttackCombo::StaticClass());
		//AbilitySystemComponent->TryActivateAbilitiesByTag(AttackComboTagContainer);
	}

}
void APlayerCharacter::AttackComboNoDelegates(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ATTACK"));
	double stam;
	GetStamina_Implementation(stam);
	FName ComboTag = "Character.AttackCombo";
	//if (stam > 15.0f && AbilitySystemComponent->ComponentHasTag(ComboTag)) {
	if (stam > 15.0f && isComboActive && !isAttacking) {
		isAttacking = true;

	}
	else if (stam > 15.0f && !isComboActive) {
		//AbilitySystemComponent->TryActivateAbilityByClass(UCharacterAttackCombo::StaticClass());
		//AbilitySystemComponent->TryActivateAbilitiesByTag(AttackComboTagContainer);
		if (MontageToPlay)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &APlayerCharacter::OnComboMontageEnded);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
			isAttacking = true;
			isComboActive = true;
		}
	}

}
void APlayerCharacter::OnComboMontageEnded(UAnimMontage* Montage, bool bInterrupted) {
	isComboActive = false;
	isAttacking = false;
}

void APlayerCharacter::Roll(const FInputActionValue& Value)
{
	double stam;
	GetStamina_Implementation(stam);
	if (stam > 15.0f) {
		FGameplayTag RollTag = UGameplayTagsManager::Get().RequestGameplayTag("Player.Abilities.Roll");
		FGameplayTagContainer RollTagContainer;
		RollTagContainer.AddTag(RollTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(RollTagContainer);
	}
}

void APlayerCharacter::BlockComplete(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BLOCK"));
	if (AbilitySystemComponent) {
		FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
		FGameplayTagContainer BlockTagContainer;
		BlockTagContainer.AddTag(BlockTag);
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(BlockTagContainer);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("END BLOCK ENDED Applied"));
		}
	}
}
void APlayerCharacter::Block(const FInputActionValue& Value)
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BLOCK"));
	}

	FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
	FGameplayTagContainer BlockTagContainer;
	BlockTagContainer.AddTag(BlockTag);
	AbilitySystemComponent->TryActivateAbilitiesByTag(BlockTagContainer);
}

void APlayerCharacter::PlayerJump(const FInputActionValue& Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JUMP"));

	double stam;
	GetStamina_Implementation(stam);

	if (stam > 15.0f) {
		FGameplayTag JumpTag = UGameplayTagsManager::Get().RequestGameplayTag("Player.Abilities.Jump");
		FGameplayTagContainer JumpTagContainer;
		JumpTagContainer.AddTag(JumpTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(JumpTagContainer);
		//ApplyGameplayEffectToSelf(UseStamina);
	}
}

void APlayerCharacter::MoveForward(const FInputActionValue& Value)
{

	if (Controller && Value.Get<float>() != 0.0f)
	{
		// Get the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		if (Value.Get<float>() < 0.0f) {
			AddMovementInput(Direction, (Value.Get<float>() / 1.8));
		}
		else {
			AddMovementInput(Direction, Value.Get<float>());
		}

	}
}

void APlayerCharacter::MoveRight(const FInputActionValue& Value)
{

	if (Controller && Value.Get<float>() != 0.0f)
	{
		// Get the forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value.Get<float>());
	}

}

void APlayerCharacter::LookRight(const FInputActionValue& Value)
{

	if (Controller && Value.Get<float>() != 0.0f)
	{

		AddControllerYawInput(Value.Get<float>());
	}
}

void APlayerCharacter::LookUp(const FInputActionValue& Value)
{

	if (Controller && Value.Get<float>() != 0.0f)
	{
		AddControllerPitchInput(Value.Get<float>());
	}
}