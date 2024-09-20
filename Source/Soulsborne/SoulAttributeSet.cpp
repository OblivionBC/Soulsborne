// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAttributeSet.h"
#include "Net/UnrealNetwork.h"

USoulAttributeSet::USoulAttributeSet()
{

}

void USoulAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Mind, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Endurance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Dexterity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Faith, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USoulAttributeSet, Luck, COND_None, REPNOTIFY_Always);
}

void USoulAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Health, OldHealth);
}

void USoulAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Mana, OldMana);
}

void USoulAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Stamina, OldStamina);
}

void USoulAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Vigor, OldVigor);
}

void USoulAttributeSet::OnRep_Mind(const FGameplayAttributeData& OldMind)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Mind, OldMind);
}

void USoulAttributeSet::OnRep_Endurance(const FGameplayAttributeData& OldEndurance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Endurance, OldEndurance);
}

void USoulAttributeSet::OnRep_Dexterity(const FGameplayAttributeData& OldDexterity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Dexterity, OldDexterity);
}

void USoulAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Intelligence, OldIntelligence);
}

void USoulAttributeSet::OnRep_Faith(const FGameplayAttributeData& OldFaith)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Faith, OldFaith);
}

void USoulAttributeSet::OnRep_Luck(const FGameplayAttributeData& OldLuck)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USoulAttributeSet, Luck, OldLuck);
}
