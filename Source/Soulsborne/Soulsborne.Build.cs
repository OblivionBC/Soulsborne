// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Soulsborne : ModuleRules
{
    public Soulsborne(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NavigationSystem", "AIModule", "GameplayTasks" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

    }
}
