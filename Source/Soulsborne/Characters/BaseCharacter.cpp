// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Sound/SoundAttenuation.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Abilities/SoulAttributeSet.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
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
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	}
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
}



const FKey ABaseCharacter::GetMovementDirection(UInputAction* Action)
{
	if (!Action) return FKey();
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return FKey();
	
	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP) return FKey();
	
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!EnhancedInputSubsystem) return FKey();
	
	TArray<FKey> KeysDown = EnhancedInputSubsystem->QueryKeysMappedToAction(Action);
	for (const FKey& Key : KeysDown) {
		if (PC->IsInputKeyDown(Key)) {
			return Key;
		}
	}
	return FKey();
}

void ABaseCharacter::printAttributes()
{
}
void ABaseCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType)
{
	if (bIsInvulnerable)
	{
		return;
	}
	if (DamagedFX){
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamagedFX, GetActorLocation(), GetActorRotation(), true);
	}
	if (AttackedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackedSound, GetActorLocation());
	}
}

void ABaseCharacter::SoulsHeal(float HealAmount)
{
	if (AbilitySystemComponent) {
		const FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(HealthAttribute);
		const float NewHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
		AbilitySystemComponent->SetNumericAttributeBase(HealthAttribute, NewHealth);
	}
}


void ABaseCharacter::SetMaxHealth(int NewMaxHealth)
{
	if (NewMaxHealth > 0) {
		MaxHealth = NewMaxHealth;
	}
}
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