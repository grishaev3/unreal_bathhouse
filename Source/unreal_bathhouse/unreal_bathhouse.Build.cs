// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class unreal_bathhouse : ModuleRules
{
    public unreal_bathhouse(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // --- ДОБАВЬТЕ ЭТИ СТРОКИ, чтобы компилятор видел подпапки ---
        PublicIncludePaths.AddRange(new string[] {
            "unreal_bathhouse/Public/Camera",
            "unreal_bathhouse/Public/Core"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
