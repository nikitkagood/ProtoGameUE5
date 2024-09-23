// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "PawnMovement.generated.h"


UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class PROTOGAME_API UPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void ApplyControlInputToVelocity(float DeltaTime) override;
	
	//Where gravity points and it's strength
	//-1 is equal to the phyisical constant of 9.81
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnMovement)
	FVector GravityVector{ 0, 0, -1 };

	EMovementMode MovementMode = MOVE_Walking;

};
