// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RPGStatsComponent.generated.h"

DECLARE_EVENT(URPGStats, FNewLevelEvent)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOGAME_API URPGStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URPGStatsComponent();

	FNewLevelEvent& GetOnNewLevel() { return NewLevelEvenVar; };

private:
	FNewLevelEvent NewLevelEvenVar;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddExperience(float exp);

	UFUNCTION()
	void IncreaseLevel();

	UFUNCTION()
	void CheckExpForLevel();

	void CalcNextLvlExp();

	//void OnNewLevel();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void SetupDefaults();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = RPG)
	int Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = RPG)
	float Experience;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = RPG)
	float Exp_next_level;

	const float FIRST_LVL_EXP = 100; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = RPG)
	float Lvl_multiplier;

};

