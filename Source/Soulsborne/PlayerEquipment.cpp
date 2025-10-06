// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEquipment.h"

// Sets default values
APlayerEquipment::APlayerEquipment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

// Called when the game starts or when spawned
void APlayerEquipment::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
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

