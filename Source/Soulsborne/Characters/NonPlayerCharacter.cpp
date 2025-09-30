// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "../Abilities/AttackCombo.h"
#include "AbilitySystemComponent.h"	
#include "GameplayAbilitySpec.h"
#include "DrawDebugHelpers.h"
#include "../SoulAttributeSet.h"
#include "GameplayTagsModule.h"
#include "SoulsPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../GameplayTags/SoulsGameplayTags.h"
#include "Perception/AIPerceptionComponent.h"
#include "Soulsborne/Components/RotationComponent.h"

/** ----------------------- Base Functions ---------------------- **/
/** Sets default values */
ANonPlayerCharacter::ANonPlayerCharacter()
{
	RotationComponent = CreateDefaultSubobject<URotationComponent>(TEXT("RotationComponent"));
	CombatComponent = CreateDefaultSubobject< UPlayerCombatComponent>("PlayerCombatComponent");
	if (!Attributes) {
		Attributes = CreateDefaultSubobject<USoulAttributeSet>("AttributesSet");
	}
}

UBehaviorTree* ANonPlayerCharacter::GetBehaviorTree()
{
	return BehaviorTree;
}

/** Called when the game starts or when spawned */
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FName rightSocketName = "righthandSocket";
	//AttatchEquipment(RHandArmamentClass, rightSocketName);
	ASoulsPlayerCharacter* Player = Cast<ASoulsPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player)
	{
		Player->OnPlayerKilled.AddDynamic(this, &ANonPlayerCharacter::OnPlayerKilledHandler);
	}
	AbilitySystemComponent->AddLooseGameplayTag(FSoulsGameplayTags::Get().Identity_Player);
}


/** Called when the character is possessed by a controller */
void ANonPlayerCharacter::PossessedBy(AController* NewController)
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

void ANonPlayerCharacter::OnRep_PlayerState()
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
void ANonPlayerCharacter::SoulsTakeDamage(float DamageAmount, FName DamageType) {
	Super::SoulsTakeDamage(DamageAmount, DamageType);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (bIsInvulnerable) return;
	if (ASC) {
		/// Here can calculate the damage based on the damage type, for now we just do the damage amount
		float CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		printAttributes();
		float NewHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()) - DamageAmount;
		printAttributes();
		ASC->SetNumericAttributeBase(HealthAttribute, NewHealth);
		CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		if (CurrentHealth <= 0)
		{
			OnDeath();
		}
		else {
			if (HitMontage)
			{
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("AtTheHitMontage"));
					GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.cantAct")));
					AnimInstance->Montage_Play(HitMontage);
				}
			}
		}
	}
}

void ANonPlayerCharacter::OnPlayerKilledHandler(AActor* KilledPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy AI detected player was killed!"));
	//this->GetMesh()->SetSimulatePhysics(true);
}

void ANonPlayerCharacter::OnDeath()
{
	Super::OnDeath();

	//this->GetMesh()->SetSimulatePhysics(true);
	DetachFromControllerPendingDestroy();
}

/** Initialize the character's attributes */
void ANonPlayerCharacter::InitializeAttributes()
{
	if (StartingStatEffect && Attributes) {
		ApplyGameplayEffectToSelf(StartingStatEffect);
	}
}

//Apply a gameplay effect to player
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

/** Called every frame */
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/** Give the character default abilities */
void ANonPlayerCharacter::GiveDefaultAbilities()
{
	// Grant abilities, but only on the server
	if (HasAuthority() && AbilitySystemComponent)
	{


		//C++ Implemented Abilities
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UAttackCombo::StaticClass(), 1, 0));
		//AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UDodge::StaticClass(), 1, 0));
	}
}