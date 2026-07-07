// Minigames.Build.cs

using UnrealBuildTool;

public class Minigames : ModuleRules
{
	public Minigames(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			// Initial Dependencies
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",

			// UI
			"UMG",

			// Level Sequence
			"LevelSequence", "MovieScene",
        });

		PublicIncludePaths.AddRange(new string[]
		{
			"Minigames",
        });

	}
}
