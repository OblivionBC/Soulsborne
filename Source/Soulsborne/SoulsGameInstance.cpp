#include "SoulsGameInstance.h"
#include "GameplayTags/SoulsGameplayTags.h"

void USoulsGameInstance::Init()
{
	Super::Init();

	FSoulsGameplayTags::InitializeNativeTags();
}
