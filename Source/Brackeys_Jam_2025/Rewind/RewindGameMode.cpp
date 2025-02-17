// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindGameMode.h"
#include "../Brackeys_Jam_2025Character.h"
#include "UObject/ConstructorHelpers.h"

ARewindGameMode::ARewindGameMode()
{
	//Set default pawn class to our blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ARewindGameMode::StartGlobalRewind()
{
	//Used for debugging in Unreal Insights Profiler
	TRACE_BOOKMARK(TEXT("ARewindGameMode::StartGlobalRewind"));

	bIsGlobalRewinding = true;
	OnGlobalRewindStarted.Broadcast();
}

void ARewindGameMode::StopGlobalRewind()
{
	//Used for debugging in Unreal Insights Profiler
	TRACE_BOOKMARK(TEXT("ARewindGameMode::StopGlobalRewind"));

	bIsGlobalRewinding = false;
	OnGlobalRewindCompleted.Broadcast();
}

void ARewindGameMode::StartGlobalFastForward()
{
	//Used for debugging in Unreal Insights Profiler
	TRACE_BOOKMARK(TEXT("ARewindGameMode::StartGlobalFastForward"));

	bIsGlobalFastForwarding = true;
	OnGlobalFastForwardStarted.Broadcast();
}

void ARewindGameMode::StopGlobalFastForward()
{
	//Used for debugging in Unreal Insights Profiler
	TRACE_BOOKMARK(TEXT("ARewindGameMode::StopGlobalFastForward"));

	bIsGlobalFastForwarding = false;
	OnGlobalFastForwardCompleted.Broadcast();
}

void ARewindGameMode::SetRewindSpeedSlowest()
{
	GlobalRewindSpeed = SlowestRewindSpeed;
}

void ARewindGameMode::SetRewindSpeedSlow()
{
	GlobalRewindSpeed = SlowRewindSpeed;
}

void ARewindGameMode::SetRewindSpeedNormal()
{
	GlobalRewindSpeed = NormalRewindSpeed;
}

void ARewindGameMode::SetRewindSpeedFast()
{
	GlobalRewindSpeed = FastRewindSpeed;
}

void ARewindGameMode::SetRewindSpeedFastest()
{
	GlobalRewindSpeed = FastestRewindSpeed;
}

void ARewindGameMode::ToggleTimeScrub()
{
	bIsGlobalTimeScrubbing = !bIsGlobalTimeScrubbing;
	if (bIsGlobalTimeScrubbing)
	{
		TRACE_BOOKMARK(TEXT("ARewindGameMode::ToggleTimeScrub - Start Time Scrubbing"));
		OnGlobalTimeScrubStarted.Broadcast();
	}
	else
	{
		TRACE_BOOKMARK(TEXT("ARewindGameMode::ToggleTimeScrub - Stop Time Scrubbing"));
		OnGlobalTimeScrubCompleted.Broadcast();
	}
}

void ARewindGameMode::ToggleGlobalTimelineVisualization()
{
	bIsGlobalTimelineVisualizationEnabled = !bIsGlobalTimelineVisualizationEnabled;
	if (bIsGlobalTimelineVisualizationEnabled)
	{
		TRACE_BOOKMARK(TEXT("ARewindGameMode::ToggleGlobalTimelineVisualization - Enable Timeline Visualization"));
		OnGlobalTimelineVisualizationEnabled.Broadcast();
	}
	else
	{
		TRACE_BOOKMARK(TEXT("ARewindGameMode::ToggleGlobalTimelineVisualization - Disable Timeline Visualization"));
		OnGlobalTimelineVisualizationDisabled.Broadcast();
	}
}
