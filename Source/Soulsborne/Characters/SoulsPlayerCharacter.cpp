// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsPlayerCharacter.h"

#include "../SoulAttributeSet.h"	
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


///////////////////////////////////////////////////             Base Functions                ///////////////////////////////////////////////////////////////

ASoulsPlayerCharacter::ASoulsPlayerCharacter()
{
	//Initialize Combat Component
	PlayerCombatComponent = CreateDefaultSubobject< UPlayerCombatComponent>("PlayerCombatComponent");

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

	SetupStimulusSource();
}

/** Called when the game starts or when spawned */
void ASoulsPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		if (HUDWidget)
		{
			PlayerHUD = CreateWidget<UUserWidget>(PlayerController, HUDWidget, "HUD");
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
			

		}
		//Static call for now, planning to implement based on a save file
		FName rightSocketName = "righthandSocket";
		AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Identity_Player);
		AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Class_Knight);
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Mapping Context Found"));
			Subsystem->AddMappingContext(MyInputMappingContext, 1);
		}
	}
}

/** Called every frame */
void ASoulsPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/** Called when the character is possessed by a controller */
void ASoulsPlayerCharacter::PossessedBy(AController* NewController)
{
	// Call the base class version
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
	printAttributes();
}

/** Called when the player state is replicated */
void ASoulsPlayerCharacter::OnRep_PlayerState()
{
	// Call the base class version
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	// Initialize the attributes and give default abilities
	InitializeAttributes();
	GiveDefaultAbilities();
	printAttributes();
}

///////////////////////////////////////////////////             Attribute Functions                ///////////////////////////////////////////////////////////

/* Initialize the character's attributes */
void ASoulsPlayerCharacter::InitializeAttributes()
{
	ApplyGameplayEffectToSelf(StartingStatEffect);
	ApplyGameplayEffectToSelf(RechargeStaminaEffect);
}

/** Combat Interface Implementations **/
void ASoulsPlayerCharacter::StartDamageTrace_Implementation() const
{
	if (PlayerCombatComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Start Damage Trace Implementation Called"));
		PlayerCombatComponent->BeginDamageTrace(RHandArmament);
	}
}

void ASoulsPlayerCharacter::EndDamageTrace_Implementation() const
{
	if (PlayerCombatComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("End Damage Trace Implementation Called"));
		PlayerCombatComponent->EndDamageTrace();
	}
}

void ASoulsPlayerCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType) {
	UE_LOG(LogTemp, Warning, TEXT("HERE IS THE DAMAGE AMOUNT %f"), DamageAmount);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	bool isBlocking = false;
	if (ASC) {
		const FGameplayTagContainer& CurrTags = ASC->GetOwnedGameplayTags();
		for (const FGameplayTag& Tag : CurrTags)
		{
			if (Tag == FGameplayTag::RequestGameplayTag(FName("State.IsBlocking")))
			{
				DamageAmount = DamageAmount * .6;
				isBlocking = true;
				break;
			}
		}
		if (isBlocking)
		{
			//Blocking Sound
		}else
		{
			//Take Damage Sound
		}
		UE_LOG(LogTemp, Warning, TEXT("We ARe IN THE DAMAGES IN PLAYER"));
		float CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		float NewHealth = CurrentHealth - DamageAmount;
		ASC->SetNumericAttributeBase(HealthAttribute, NewHealth);
		UE_LOG(LogTemp, Warning, TEXT("PLAYER Health IS HEREEEEEEEE %f"), ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
		CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		if (CurrentHealth <= 0 && !bIsDead)
		{
			Die();
		}
		else {
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

///////////////////////////////////////////////////             Abilities                ///////////////////////////////////////////////////////////////////
//Apply a gameplay effect to player
void ASoulsPlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply) {
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
		//C++ Implemented Abilities
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UAttackCombo::StaticClass(), 1, 0));
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UDodge::StaticClass(), 1, 0));
	}
}

///////////////////////////////////////////////////             Inputs                //////////////////////////////////////////////////////////////////////

void ASoulsPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::MoveRight, IA_MoveRight);
		EnhancedInputComponent->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::MoveForward, IA_MoveForward);

		EnhancedInputComponent->BindAction(IA_LookRight, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::LookRight);
		EnhancedInputComponent->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::LookUp);
		//Abilities
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::PlayerJump);
		EnhancedInputComponent->BindAction(IA_LockCamera, ETriggerEvent::Started, this, &ASoulsPlayerCharacter::LockCamera);

		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Triggered, this, &ASoulsPlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Ongoing, this, &ASoulsPlayerCharacter::Block);
		EnhancedInputComponent->BindAction(IA_Block, ETriggerEvent::Completed, this, &ASoulsPlayerCharacter::BlockComplete);

	}
}

//In Combat Component now, will be migrated to Gameplay Ability Soon
void ASoulsPlayerCharacter::LockCamera(const FInputActionValue& Value) {

	if (PlayerCombatComponent) {
		PlayerCombatComponent->TargetLockCamera();
	}
}

void ASoulsPlayerCharacter::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimulusSource) {
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

//Blueprint for now, will be migrated to Gameplay Ability Soon
void ASoulsPlayerCharacter::BlockComplete(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		FGameplayTag BlockTag = UGameplayTagsManager::Get().RequestGameplayTag("Character.IsBlocking");
		FGameplayTagContainer BlockTagContainer;
		BlockTagContainer.AddTag(BlockTag);
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(BlockTagContainer);
	}
}

//Blueprint for now, will be migrated to Gameplay Ability Soon
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
	double stam;
	ABaseCharacter::GetStamina_Implementation(stam);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Jump"));
	if (stam > 15.0f) {
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
		if (Value.Get<float>() < 0.0f) {
			AddMovementInput(Direction, (Value.Get<float>() / 1.4));
		}
		else {
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