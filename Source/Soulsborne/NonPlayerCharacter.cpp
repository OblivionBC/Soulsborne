// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "AttackCombo.h"
#include "AbilitySystemComponent.h"	
#include "GameplayAbilitySpec.h"
#include "DrawDebugHelpers.h"
#include "SoulAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagsModule.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
/** ----------------------- Base Functions ---------------------- **/
/** Sets default values */
ANonPlayerCharacter::ANonPlayerCharacter()
{
	//Initialize Combat Component
	CombatComponent = CreateDefaultSubobject< UPlayerCombatComponent>("PlayerCombatComponent");
	if (!Attributes) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("THERE WAS NO ATTRIBVUTES"));
		Attributes = CreateDefaultSubobject<USoulAttributeSet>("AttributesSet");
	}
}

/** Called when the game starts or when spawned */
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BEGIN PLAY CALLED"));
	FName rightSocketName = "righthandSocket";
	AttatchEquipment(RHandArmamentClass, rightSocketName);
}


/** Called when the character is possessed by a controller */
void ANonPlayerCharacter::PossessedBy(AController* NewController)
{
	// Call the base class version
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AbilitySystemInit"));
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
void ANonPlayerCharacter::SoulsTakeDamage() {
	Super::SoulsTakeDamage();
	UE_LOG(LogTemp, Warning, TEXT("We ARe IN THE DAMAGE BEFORE IUMP"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IN DAMAGED BEFORE THE IMP"));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC) {
		UE_LOG(LogTemp, Warning, TEXT("We ARe IN THE DAMAGES"));
		float CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		FGameplayAttribute HealthAttribute = USoulAttributeSet::GetHealthAttribute();
		printAttributes();
		float NewHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()) - 90.0f;
		printAttributes();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IN DAMAGED"));
		ASC->SetNumericAttributeBase(HealthAttribute, NewHealth);
		UE_LOG(LogTemp, Warning, TEXT("Health IS HEREEEEEEEE %f"), ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
		CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
		if (CurrentHealth <= 0)
		{
			this->GetMesh()->SetSimulatePhysics(true);
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


/** Initialize the character's attributes */
void ANonPlayerCharacter::InitializeAttributes()
{
	if (StartingStatEffect && Attributes) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GOING TO APPLY"));
		ApplyGameplayEffectToSelf(StartingStatEffect);
	}
}

//Apply a gameplay effect to player
void ANonPlayerCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> EffectToApply) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IN APPLY GAMEPLAY"));
	if (AbilitySystemComponent && EffectToApply) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("APPLYING STARTING STATS"));
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("APPLYING Effect"));
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