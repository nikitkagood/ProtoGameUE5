// Copyright Epic Games, Inc. All Rights Reserved.

#include "FpsGameMode.h"
#include "UI/FpsHUD.h"
#include "Character/GameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFpsGameMode::AFpsGameMode() : Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Characters/BP_GameCharacterBase"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFpsHUD::StaticClass();
}
