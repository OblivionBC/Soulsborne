// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEquipment.h"

APlayerEquipment::APlayerEquipment()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMesh->SetupAttachment(Root);
	StartAttackTrace = CreateDefaultSubobject<UArrowComponent>("StartAttackTrace");
	StartAttackTrace->SetupAttachment(SkeletalMesh);
	EndAttackTrace = CreateDefaultSubobject<UArrowComponent>("EndAttackTrace");
	EndAttackTrace->SetupAttachment(SkeletalMesh);

	minDamage = 2;
	maxDamage = 5;
	DamageType = FName(TEXT("Neutral"));
}

void APlayerEquipment::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UArrowComponent* APlayerEquipment::GetStartAttackTrace() {
	return StartAttackTrace;
}
UArrowComponent* APlayerEquipment::GetEndAttackTrace() {
	return EndAttackTrace;
}

