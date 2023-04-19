// Nikita Belov, All rights reserved


#include "Vehicle/VehiclePawnBase.h"

AVehiclePawnBase::AVehiclePawnBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AVehiclePawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

//void AVehiclePawnBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void AVehiclePawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

