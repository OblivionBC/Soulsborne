#include "SoulsGameplayTags.h"
#include "NativeGameplayTags.h"
#include "GameplayTagsManager.h"

FSoulsGameplayTags FSoulsGameplayTags::Instance;
UE_DEFINE_GAMEPLAY_TAG(TAG_Boss_Rampage, "Boss.Rampage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Enemy_Regular, "Enemy.Regular");
UE_DEFINE_GAMEPLAY_TAG(TAG_Class_Knight, "Class.Knight");
UE_DEFINE_GAMEPLAY_TAG(TAG_Identity_Player, "Identity.Player");
UE_DEFINE_GAMEPLAY_TAG(TAG_Identity_Enemy, "Identity.Enemy");
void FSoulsGameplayTags::InitializeNativeTags()
{
	Instance.Boss_Rampage = TAG_Boss_Rampage;
	Instance.Enemy_Regular = TAG_Enemy_Regular;
	Instance.Class_Knight = TAG_Class_Knight;
	Instance.Identity_Player = TAG_Identity_Player;
	Instance.Identity_Enemy = TAG_Identity_Enemy;
	
	UGameplayTagsManager::Get().DoneAddingNativeTags();
}
