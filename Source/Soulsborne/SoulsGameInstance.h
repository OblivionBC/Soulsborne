#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SoulsGameInstance.generated.h"

UCLASS()
class USoulsGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};