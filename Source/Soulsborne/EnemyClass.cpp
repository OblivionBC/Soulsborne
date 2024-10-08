// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyClass.h"
#include "AbilitySystemComponent.h"		
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SoulAttributeSet.h"					

/** ----------------------- Base Functions ---------------------- **/
/** Sets default values */
AEnemyClass::AEnemyClass()
{
	// Initialize the ability system component
	GetAbilitySystemComponent();
	printAttributes();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AEnemyClass::printAttributes()
{
	UE_LOG(LogTemp, Warning, TEXT("Max Health: %d"), MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Max Stamina: %d"), MaxStamina);
	UE_LOG(LogTemp, Warning, TEXT("Max Mana: %d"), MaxMana);
	UE_LOG(LogTemp, Warning, TEXT("Max Stat: %d"), MaxStat);
}

/** Called when the game starts or when spawned */
void AEnemyClass::BeginPlay()
{
	Super::BeginPlay();
	printAttributes();
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter) {
		PlayerCharacter->DamageDealt.AddDynamic(this, &AEnemyClass::Grow);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Player Is NUll %d"), MaxStat);
	}

}
void AEnemyClass::Damaged() {
	Super::Damaged();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	//Result = AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());
	if (ASC) {
		float CurrentHealth = ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute());

		//FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DAMAGE Applied"));
			UE_LOG(LogTemp, Warning, TEXT("Health %f"), ASC->GetNumericAttribute(USoulAttributeSet::GetHealthAttribute()));
		}
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
					AnimInstance->Montage_Play(HitMontage);
				}
			}
		}
	}
}
/** Called every frame */
void AEnemyClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyClass::Grow(AActor* DamagedActor, float Damage, AController* InstigatedBy)
{
	SetActorScale3D(GetActorScale3D() + 0.1f);
	Damaged();
}
