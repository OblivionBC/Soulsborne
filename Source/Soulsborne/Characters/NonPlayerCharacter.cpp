// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "AbilitySystemComponent.h"	
#include "GameplayAbilitySpec.h"
#include "DrawDebugHelpers.h"
#include "GameplayTagsModule.h"
#include "SoulsPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Soulsborne/Abilities/AttackCombo.h"
#include "Soulsborne/Abilities/SoulAttributeSet.h"
#include "Soulsborne/Components/RotationComponent.h"
#include "Soulsborne/GameplayTags/SoulsGameplayTags.h"

ANonPlayerCharacter::ANonPlayerCharacter()
{
	RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("RotationComponent"));
	if (!Attributes) {
		Attributes = CreateDefaultSubobject<USoulAttributeSet>("AttributesSet");
	}
}

UBehaviorTree* ANonPlayerCharacter::GetBehaviorTree()
{
	return BehaviorTree;
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player)
	{
		Player->OnPlayerKilled.AddDynamic(this, &ANonPlayerCharacter::OnPlayerKilledHandler);
	}
	AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Identity_Player);
}


void ANonPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	InitializeAttributes();
	GiveDefaultAbilities();
}

void ANonPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent) {
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	InitializeAttributes();
	GiveDefaultAbilities();
}
void ANonPlayerCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType) {
	Super::SoulsTakeDamage(DamageAmount, DamageType);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (bIsInvulnerable) return;
	if (ASC) {
		const FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		const float CurrentHealth = ASC->GetNumericAttribute(HealthAttribute);
		const float NewHealth = CurrentHealth - DamageAmount;
		ASC->SetNumericAttributeBase(HealthAttribute, NewHealth);
		const float ResultHealth = ASC->GetNumericAttribute(HealthAttribute);
		if (ResultHealth <= 0)
		{
			OnDeath();
		}
		else {
			if (HitMontage)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.cantAct")));
					AnimInstance->Montage_Play(HitMontage);
				}
			}
		}
	}
}

void ANonPlayerCharacter::OnPlayerKilledHandler(AActor* KilledPlayer)
{
}

void ANonPlayerCharacter::OnDeath()
{
	Super::OnDeath();
	DetachFromControllerPendingDestroy();
}

void ANonPlayerCharacter::InitializeAttributes()
{
	if (StartingStatEffect && Attributes) {
		ApplyGameplayEffectToSelf(StartingStatEffect);
	}
}

void ANonPlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply) {
	if (AbilitySystemComponent && EffectToApply) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANonPlayerCharacter::GiveDefaultAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UAttackCombo::StaticClass(), 1, 0));
	}
}