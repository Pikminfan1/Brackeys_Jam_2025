// Copyright Epic Games, Inc. All Rights Reserved.

#include "Brackeys_Jam_2025GameMode.h"
#include "Brackeys_Jam_2025Character.h"
#include "UObject/ConstructorHelpers.h"

ABrackeys_Jam_2025GameMode::ABrackeys_Jam_2025GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
