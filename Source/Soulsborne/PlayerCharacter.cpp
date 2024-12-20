

#include "PlayerCharacter.h"	
#include "SoulAttributeSet.h"	
#include "CharacterAttackCombo.h"
#include "DynamicTakeDamage.h"
#include "PlayerCombatInterface.h"
#include "PlayerEquipment.h"
#include "ProgressBarInterface.h"
#include "CharacterDodge.h"
#include "AttackCombo.h"

#include "AbilitySystemComponent.h"	
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagsModule.h"
#include "GameplayAbilitySpec.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
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

	//Initialize Combat Component
	PlayerCombatComponent = CreateDefaultSubobject< UPlayerCombatComponent>("PlayerCombatComponent");

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
	}
}
void APlayerCharacter::OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) {
	if (!isAttacking)
	{
		//GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, MontageToPlay);
		isComboActive = false;
		isAttacking = false;
		return;
	}
	isAttacking = false;
}
/** Called every frame */
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Start Damage Trace Implementation Called"));
		PlayerCombatComponent->BeginDamageTrace(RHandArmament);
	}
}

void APlayerCharacter::EndDamageTrace_Implementation() const
{
	if (PlayerCombatComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("End Damage Trace Implementation Called"));
		PlayerCombatComponent->EndDamageTrace();
	}
}
/*** --------------------------------- Abilities ------------------------------- ***/
//Apply a gameplay effect to player
void APlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply) {
	if (AbilitySystemComponent && EffectToApply) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		//EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
		//C++ Implemented Abilities
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UAttackCombo::StaticClass(), 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UCharacterDodge::StaticClass(), 1, 0));
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ATTACHING")));
		FActorSpawnParameters SpawnParams;
		LHandArmament = GetWorld()->SpawnActor<AActor>(LHandArmamentClass, LeftSocketT, SpawnParams);
		RHandArmament = GetWorld()->SpawnActor<AActor>(RHandArmamentClass, RightSocketT, SpawnParams);

		LHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, leftSocketName);
		RHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, rightSocketName);

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
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRight, IA_MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForward, IA_MoveForward);

		EnhancedInputComponent->BindAction(IA_LookRight, ETriggerEvent::Triggered, this, &APlayerCharacter::LookRight);
		EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &APlayerCharacter::LookUp);
		//Ability Movement
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerJump);
		EnhancedInputComponent->BindAction(IA_LockCamera, ETriggerEvent::Started, this, &APlayerCharacter::LockCamera);

		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Triggered, this, &APlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Ongoing, this, &APlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Completed, this, &APlayerCharacter::BlockComplete);

		//EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &APlayerCharacter::Roll);

	}
}

void APlayerCharacter::LockCamera(const FInputActionValue& Value) {

	if (PlayerCombatComponent) {
		PlayerCombatComponent->TargetLockCamera();
	}
}

//Returns the key if the specified input action's key is being pressed
const FKey APlayerCharacter::GetMovementDirection(UInputAction* Action)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (EnhancedInputSubsystem)
			{
				TArray<FKey> KeysDown = EnhancedInputSubsystem->QueryKeysMappedToAction(Action);
				for (const FKey Key : KeysDown) {
					if (PC->IsInputKeyDown(Key)) {
						return Key;
					}
				}
			}
		}
	}
	FKey KeyDown;
	return KeyDown;
}


//Being migrated to C++ Ability Currently
void APlayerCharacter::Roll(const FInputActionValue& Value)
{
	double stam;
	GetStamina_Implementation(stam);
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Player.Abilities.Roll"));
	if (stam > 15.0f && PlayerCombatComponent && !AbilitySystemComponent->HasMatchingGameplayTag(Tag)) {
		AbilitySystemComponent->TryActivateAbilityByClass(UCharacterDodge::StaticClass());
		ApplyGameplayEffectToSelf(UseStamina);

		//AbilitySystemComponent->TryActivateAbilitiesByTag(RollTagContainer);
	}
}

void APlayerCharacter::BlockComplete(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
		FGameplayTagContainer BlockTagContainer;
		BlockTagContainer.AddTag(BlockTag);
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(BlockTagContainer);
	}
}

void APlayerCharacter::Block(const FInputActionValue& Value)
{

	FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
	FGameplayTagContainer BlockTagContainer;
	BlockTagContainer.AddTag(BlockTag);
	AbilitySystemComponent->TryActivateAbilitiesByTag(BlockTagContainer);
}

void APlayerCharacter::PlayerJump(const FInputActionValue& Value)
{
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

void APlayerCharacter::MoveForward(const FInputActionValue& Value, UInputAction* Action)
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
		DirectionKey = GetMovementDirection(Action);
	}
}

void APlayerCharacter::MoveRight(const FInputActionValue& Value, UInputAction* Action)
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