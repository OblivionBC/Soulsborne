// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Sound/SoundAttenuation.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/ENUM/DamageTypeENum.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
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
	bIsDead = true;
	bIsInvulnerable = true;
	if (DeathSound)
	{
		USoundAttenuation * SoundAttenuation = NewObject<USoundAttenuation>();
		SoundAttenuation->Attenuation.bAttenuate = true;
		SoundAttenuation->Attenuation.bSpatialize = true;
		SoundAttenuation->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		SoundAttenuation->Attenuation.FalloffDistance = 1000.0f;
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DeathSound,
			GetActorLocation(),
			GetActorRotation(),
			1.0f,
			1.0f,
			0.0f,
			SoundAttenuation
			);
	}
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
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
void ABaseCharacter::SoulsTakeDamage(float DamageAmount, EDamageType DamageType)
{
	if (bIsInvulnerable)
	{
		return;
	}
	if (this->DamagedFX){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			this->DamagedFX,
			this->GetActorLocation(),
			this->GetActorRotation(),
			true
			);
	}
	if (this->AttackedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this, this->AttackedSound, this->GetActorLocation());
	}
}

void ABaseCharacter::SoulsHeal(float HealAmount)
{
	if (AbilitySystemComponent) {
	
		FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
        float NewHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
        AbilitySystemComponent->SetNumericAttributeBase(HealthAttribute, NewHealth);
	}
	
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
	UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *this->GetName());

	if (AbilitySystemComponent) {
		Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()) / MaxHealth;
	}
	else {
		Result = 0.0f;
	}
}