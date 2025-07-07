// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Initialize the ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	// Initialize the attribute set
	Attributes = CreateDefaultSubobject<USoulAttributeSet>("AttributesSet");
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

void ABaseCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(ClassTags);
}

void ABaseCharacter::OnDeath()
{
}


//Used to attatch equipment to the socket name specified, currently static for development purposes
void ABaseCharacter::AttatchEquipment(TSubclassOf<AActor> Equipment, FName socketName) {
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (PlayerMesh) {
		FName rightSocketName = "righthandSocket";
		FName leftSocketName = "lefthandSocket";
		FTransform LeftSocketT = PlayerMesh->GetSocketTransform(leftSocketName);
		FTransform RightSocketT = PlayerMesh->GetSocketTransform(rightSocketName);
		FActorSpawnParameters SpawnParams;
		if (LHandArmamentClass) {
			LHandArmament = GetWorld()->SpawnActor<AActor>(LHandArmamentClass, LeftSocketT, SpawnParams);
			LHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, leftSocketName);
		}
		if (RHandArmamentClass) {
			RHandArmament = GetWorld()->SpawnActor<AActor>(RHandArmamentClass, RightSocketT, SpawnParams);
			RHandArmament->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, rightSocketName);
		}
	}
}

//Returns the key if the specified input action's key is being pressed
const FKey ABaseCharacter::GetMovementDirection(UInputAction* Action)
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

void ABaseCharacter::printAttributes()
{
	double result = 0;
	GetHealth_Implementation(result);
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), result);
	GetStamina_Implementation(result);
	UE_LOG(LogTemp, Warning, TEXT(" Stamina: %f"), result);
	GetMana_Implementation(result);
	UE_LOG(LogTemp, Warning, TEXT(" Mana: %f"), result);
	UE_LOG(LogTemp, Warning, TEXT("Max Stat: %d"), MaxStat);
}
void ABaseCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType)
{
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::SetMaxHealth(int NewMaxHealth)
{
	if (NewMaxHealth > 0) {
		MaxHealth = NewMaxHealth;
	}
}
/** Player Progress Bar Interface Implementations **/
void ABaseCharacter::GetStamina_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void ABaseCharacter::GetMana_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetManaAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void ABaseCharacter::GetHealth_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
	}
	else {
		Result = 0.0f;
	}
}
void ABaseCharacter::GetStaminaAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetStaminaAttribute()) / MaxStamina;
	}
	else {
		Result = 0.0f;
	}
}
void ABaseCharacter::GetManaAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetManaAttribute()) / MaxMana;
	}
	else {
		Result = 0.0f;
	}
}

void ABaseCharacter::GetHealthAsRatio_Implementation(double& Result) const {
	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()) / MaxHealth;
	}
	else {
		Result = 0.0f;
	}
}