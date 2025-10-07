#include "BossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../Characters/BossCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Soulsborne/Characters/SoulsPlayerCharacter.h"

ABossAIController::ABossAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	SetupPerceptionSystem();
}


void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(InPawn))
	{
		UBehaviorTree* BTAsset = Boss->GetBehaviorTree();
		if (BTAsset && BlackboardComp)
		{
			if (BlackboardComp->InitializeBlackboard(*BTAsset->BlackboardAsset))
			{
				// Cache key IDs from names
				TargetActorKey = BlackboardComp->GetKeyID("targetActor");
				bIsEnragedKey = BlackboardComp->GetKeyID("bIsEnraged");
				PhaseKey = BlackboardComp->GetKeyID("Phase");
				HealthPercentKey = BlackboardComp->GetKeyID("HealthPercent");
				ShouldUseAbilityKey = BlackboardComp->GetKeyID("ShouldUseAbility");
				DistanceKey = BlackboardComp->GetKeyID("DistanceToTarget");
				IsAttackingKey = BlackboardComp->GetKeyID("IsAttacking");
				bCombatEngagedKey = BlackboardComp->GetKeyID("bCombatEngaged");
				bIsPlayerDead = BlackboardComp->GetKeyID("bIsPlayerDead");


				RunBehaviorTree(BTAsset);
				UE_LOG(LogTemp, Display, TEXT("Behaviour Tree Started In Controller!"));
			}
		}else
		{
			UE_LOG(LogTemp, Display, TEXT("BehaviorTreeComp->InitializeBlackboard()"));
		}
		
	}
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABossAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig) {
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		SightConfig->SightRadius = 1500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABossAIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void ABossAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (!BlackboardComp) return;
	
	if (Cast<ASoulsPlayerCharacter>(Actor)) {
		const bool bSensed = Stimulus.WasSuccessfullySensed();
		BlackboardComp->SetValueAsBool("CanSeePlayer", bSensed);
		if (bSensed)
		{
			SetDistanceToTarget(FVector::Dist(GetPawn()->GetActorLocation(), Actor->GetActorLocation()));
			BlackboardComp->SetValueAsObject("targetActor", Actor);
			AttackTarget = Actor;
		}
		
	}
}

void ABossAIController::SetDistanceToTarget(float Distance)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsFloat(BlackboardComp->GetKeyName(DistanceKey), Distance);
	}
}
void ABossAIController::SetIsAttacking(bool bIsAttacking)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(BlackboardComp->GetKeyName(IsAttackingKey), bIsAttacking);
	}
}


void ABossAIController::SetTargetActor(AActor* Target)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(BlackboardComp->GetKeyName(TargetActorKey), Target);
	}
}

void ABossAIController::SetIsEnraged(bool bIsEnraged)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(BlackboardComp->GetKeyName(bIsEnragedKey), bIsEnraged);
	}
}

void ABossAIController::UpdateHealthPercent(float Percent)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsFloat(BlackboardComp->GetKeyName(HealthPercentKey), Percent);
	}
}

void ABossAIController::SetPhase(int32 Phase)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsInt(BlackboardComp->GetKeyName(PhaseKey), Phase);
	}
}

void ABossAIController::SetbIsCombatEngaged(bool bEngaged)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(BlackboardComp->GetKeyName(bCombatEngagedKey), bEngaged);
	}
}

void ABossAIController::SetbIsPlayerDead(bool bIsDead)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(BlackboardComp->GetKeyName(bIsPlayerDead), bIsDead);
	}
}

void ABossAIController::SetShouldUseAbility(bool bValue)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(BlackboardComp->GetKeyName(ShouldUseAbilityKey), bValue);
	}
}

int ABossAIController::GetPhase()
{
	if (BlackboardComp)
	{
		return BlackboardComp->GetValueAsInt(BlackboardComp->GetKeyName(PhaseKey));
	}
	return 1;
}

