// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCombatComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UPlayerCombatComponent::UPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UPlayerCombatComponent::DamageTrace()
{
	// Implement sphere trace logic here
}

void UPlayerCombatComponent::BeginDamageTrace()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerCombatComponent::DamageTrace, 0.1f, true);
}

void UPlayerCombatComponent::EndDamageTrace()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}