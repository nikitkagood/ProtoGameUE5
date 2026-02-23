// Nikita Belov, All rights reserved


#include "RPGStatsComponent.h"

// Sets default values for this component's properties
URPGStatsComponent::URPGStatsComponent() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = 0.5f; //half a second

	SetupDefaults();
}


// Called when the game starts
void URPGStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void URPGStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URPGStatsComponent::AddExperience(float exp)
{
	Experience += exp;
}

void URPGStatsComponent::IncreaseLevel()
{
	Level++;
	GetOnNewLevel().Broadcast();
}

void URPGStatsComponent::CheckExpForLevel()
{
	if(Experience >= Exp_next_level)
	{
		IncreaseLevel();
		CalcNextLvlExp();
	}
}

void URPGStatsComponent::CalcNextLvlExp()
{
	Exp_next_level = Level * Lvl_multiplier * FIRST_LVL_EXP;
}

void URPGStatsComponent::SetupDefaults()
{
	Level = 1;
	Experience = 0;
	Exp_next_level = 0;
	Lvl_multiplier = 1.3f;

	CalcNextLvlExp();
}

