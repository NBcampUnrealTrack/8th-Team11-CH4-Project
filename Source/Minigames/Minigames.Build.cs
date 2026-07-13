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
			"UMG","Slate", "SlateCore",

			// Level Sequence
			"LevelSequence", "MovieScene", 

			// Particle Effect
			"Niagara", 
        });

		PublicIncludePaths.AddRange(new string[]
		{
			"Minigames",
        });

	}
}
