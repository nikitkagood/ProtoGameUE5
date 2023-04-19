// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/VehiclePawnBase.h"

#include "WheleedVehiclePawn.generated.h"

//It's available because ChaosVehicles is added to .Build.cs modules dependencies
class UChaosWheeledVehicleMovementComponent;

UCLASS()
class PROTOGAME_API AWheleedVehiclePawn : public AVehiclePawnBase
{
	GENERATED_BODY()
	
	/** vehicle simulation component */
	UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChaosWheeledVehicleMovementComponent> VehicleMovementComponent;

public:
	UChaosWheeledVehicleMovementComponent* GetVehicleMovementComponent() const { return VehicleMovementComponent; }
};
