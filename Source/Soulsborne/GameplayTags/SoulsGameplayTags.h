#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Boss_Rampage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Enemy_Regular);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Class_Knight);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Identity_Player);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Identity_Enemy);
struct FSoulsGameplayTags
{
public:
	static const FSoulsGameplayTags& Get() {return Instance;}
	static void InitializeNativeTags();

	// Classes
	FGameplayTag Boss_Rampage;
	FGameplayTag Enemy_Regular;
	FGameplayTag Class_Knight;

	// Identities
	FGameplayTag Identity_Player;
	FGameplayTag Identity_Enemy;
	

private:
	static FSoulsGameplayTags Instance;
};
