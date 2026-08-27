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

#pragma once

#include "WwiseDefines.h"

#include "Containers/UnrealString.h"
#include "Engine/EngineTypes.h"

namespace EWwiseItemType
{

	enum Type
	{
		Event,
		AuxBus,
		Switch,
		State,
		GameParameter,
		DialogueEvent,
		EffectShareSet,
		Trigger,
		AcousticTexture,
		AudioDeviceShareSet,
		ActorMixer,
		Bus,
		Project,
		StandaloneWorkUnit,
		NestedWorkUnit,
		PhysicalFolder,
		Folder,
		Sound,
		SwitchContainer,
		RandomSequenceContainer,
		BlendContainer,
		MotionBus,
		StateGroup,
		SwitchGroup,
		InitBank,
		AudioNode,
		LastWwiseBrowserType = AudioDeviceShareSet,

		None = -1,
	};

	static const FString EventsBrowserName = TEXT("Events");
	static const FString DialogueEventsBrowserName = TEXT("Dynamic Dialogue");
	static const FString BussesBrowserName = TEXT("Busses");
	static const FString AcousticTexturesBrowserName = TEXT("Virtual Acoustics");
	static const FString AudioDeviceShareSetBrowserName = TEXT("Device ShareSets");
	static const FString StatesBrowserName = TEXT("States");
	static const FString SwitchesBrowserName = TEXT("Switches");
	static const FString GameParametersBrowserName = TEXT("Game Parameters");
	static const FString TriggersBrowserName = TEXT("Triggers");
	static const FString ShareSetsBrowserName =	TEXT("Effect ShareSets");
	static const FString OrphanAssetsBrowserName = TEXT("Orphan Assets");

	//Name to show in the Browser
	static const FString BrowserDisplayNames[] = {
		EventsBrowserName,
		DialogueEventsBrowserName,
		BussesBrowserName,
		AcousticTexturesBrowserName,
		AudioDeviceShareSetBrowserName,
		StatesBrowserName,
		SwitchesBrowserName,
		GameParametersBrowserName,
		TriggersBrowserName,
		ShareSetsBrowserName,
		OrphanAssetsBrowserName
	};

	//Name of the folder containing the work units of this WwiseObjectType
	static const FString FolderNames[] = {
		TEXT("Events"),
#if WWISE_2025_1_OR_LATER
		TEXT("Busses"),
#else
		TEXT("Master-Mixer Hierarchy"),
#endif
		TEXT("Switches"),
		TEXT("States"),
		TEXT("Game Parameters"),
		TEXT("Dynamic Dialogue"),
		TEXT("Effects"),
		TEXT("Triggers"),
		TEXT("Virtual Acoustics"),
#if WWISE_2025_1_OR_LATER
		TEXT("Devices")
#else
		TEXT("Audio Devices")
#endif
	};

	inline Type FromString(const FString& ItemName)
	{
		struct TypePair
		{
			FString Name;
			Type Value;
		};

		static const TypePair ValidTypes[] = {
			{TEXT("AcousticTexture"), Type::AcousticTexture},
			{TEXT("AudioDevice"), Type::AudioDeviceShareSet},
			{TEXT("ActorMixer"), Type::ActorMixer},
			{TEXT("AuxBus"), Type::AuxBus},
			{TEXT("BlendContainer"), Type::BlendContainer},
			{TEXT("Bus"), Type::Bus},
			{TEXT("DialogueEvent"), Type::DialogueEvent},
			{TEXT("Event"), Type::Event},
			{TEXT("Folder"), Type::Folder},
			{TEXT("GameParameter"), Type::GameParameter},
			{TEXT("MotionBus"), Type::MotionBus},
			{TEXT("PhysicalFolder"), Type::PhysicalFolder},
			{TEXT("Project"), Type::Project},
			{TEXT("RandomSequenceContainer"), Type::RandomSequenceContainer},
			{TEXT("Sound"), Type::Sound},
			{TEXT("State"), Type::State},
			{TEXT("StateGroup"), Type::StateGroup},
			{TEXT("Switch"), Type::Switch},
			{TEXT("SwitchContainer"), Type::SwitchContainer},
			{TEXT("SwitchGroup"), Type::SwitchGroup},
			{TEXT("Trigger"), Type::Trigger},
			{TEXT("WorkUnit"), Type::StandaloneWorkUnit},
			{TEXT("Effect"), Type::EffectShareSet},

		};

		for (const auto& type : ValidTypes)
		{
			if (type.Name == ItemName)
			{
				return type.Value;
			}
		}

		return Type::None;
	}
};
