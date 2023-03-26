// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "VehicleInfo.generated.h"

UENUM(BlueprintType)
enum class DifferentialType : uint8
{
	//Non-customizable
	//No differential, always unlocked, can't change anything
	None							UMETA(DisplayName = "None"),
	FrontRearCenterUnchangeable		UMETA(DisplayName = "Front Rear Center (unchangeable)"),
	//Center isn't present at all
	FrontRearUnchangealbe			UMETA(DisplayName = "Front Rear (unchangeable)"),
	FrontUnchangealbe				UMETA(DisplayName = "Front (unchangeable)"),
	RearUnchangealbe				UMETA(DisplayName = "Rear (unchangeable)"),

	//Customizable
	//Fully customizable
	FrontRearCenter					UMETA(DisplayName = "FrontRearCenter"),
	FrontRear						UMETA(DisplayName = "Front Rear"),
	//No differential on rear, customizable front
	Front							UMETA(DisplayName = "Front"),
	Rear							UMETA(DisplayName = "Rear"),

	//Partially customizeable
	CenterUnchangeableFrontRear		UMETA(DisplayName = "Center (unchangeable) Front Rear "),
};


//Contains only values which aren't present in VehicleMovementComponent
USTRUCT(BlueprintType)
struct PROTOGAME_API FVehicleInfo : public FTableRowBase
{
	GENERATED_BODY()

	//Might be for informational purposes only. Since real top spead is determined by torque/RPM/gearing setup
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	float TopSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	DifferentialType differential_type;

	//TArray VehicleSeats
	//TArray InventoryComponents or make InventoryManager

	//Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* EngineSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio, meta = (AllowPrivateAccess = true))
	USoundBase* GearShiftSound;
};