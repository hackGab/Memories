/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

public class WwiseUEPlatform_WinGDK : WwiseUEPlatform_Windows
{
	public WwiseUEPlatform_WinGDK(ReadOnlyTargetRules in_TargetRules, string in_ThirdPartyFolder) : base(in_TargetRules, in_ThirdPartyFolder) {}

	protected virtual Version FirstSoundEngineVersionWithoutWinGC { get { return new Version(9999, 0, 0, 0); } }

	public override bool SupportsAkAutobahn { get { return false; } }
	
	public override string PlatformPrefix { get { return "x64"; } }

	public override List<string> GetPublicLibraryPaths()
	{
		var SoundEngineVersion = WwiseSoundEngineVersion.GetInstance(ThirdPartyFolder);
		if (SoundEngineVersion < FirstSoundEngineVersionWithoutWinGC)
		{
			return new List<string> { Path.Combine(ThirdPartyFolder, AkPlatformLibDir, WwiseConfigurationDir, "lib") };
		}
		return base.GetPublicLibraryPaths();
		
	}

	public override string AkPlatformLibDir
	{
		get
		{
			var SoundEngineVersion = WwiseSoundEngineVersion.GetInstance(ThirdPartyFolder);
			if (SoundEngineVersion < FirstSoundEngineVersionWithoutWinGC)
			{
				return "WinGC_vc160";
			}
			return base.AkPlatformLibDir;
		}
	}

	public override List<string> GetPublicDefinitions()
	{
		var SoundEngineVersion = WwiseSoundEngineVersion.GetInstance(ThirdPartyFolder);
		if (SoundEngineVersion < FirstSoundEngineVersionWithoutWinGC)
		{
			return new List<string>
			{
				"AK_WINDOWSGC",
				"AK_WINGC_VS_VERSION=\"" + GetVisualStudioVersion() + "\""
			};
		}
		return base.GetPublicDefinitions();
	}
	
	private static void CheckCompilerVersion(ref string Version, WindowsCompiler Compiler, string LongVersionName, string ShortVersionName)
	{
		try
		{
			if (Compiler == (WindowsCompiler)Enum.Parse(typeof(WindowsCompiler), LongVersionName))
				Version = ShortVersionName;
		}
		catch
		{
		}
	}

	private string GetVisualStudioVersion()
	{
		string VSVersion = "vc170";
		var Compiler = Target.WindowsPlatform.Compiler;
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2022", "vc170");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2019", "vc160");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2017", "vc150");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2015", "vc140");
		CheckCompilerVersion(ref VSVersion, Compiler, "VisualStudio2013", "vc120");
		return VSVersion;
	}
}
