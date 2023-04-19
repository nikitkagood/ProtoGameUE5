// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "VehiclePawnBase.generated.h"

UCLASS(abstract, BlueprintType)
class PROTOGAME_API AVehiclePawnBase : public APawn
{
	GENERATED_BODY()

public:
	AVehiclePawnBase();

private:
	/**  The main skeletal mesh associated with this Vehicle */
	//UPROPERTY(Category = Vehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<class USkeletalMeshComponent> Mesh;

protected:
	virtual void BeginPlay() override;

public:	
	/** Returns Mesh subobject **/
	//class USkeletalMeshComponent* GetMesh() const { return Mesh; }

	//virtual void Tick(float DeltaTime) override;

	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
