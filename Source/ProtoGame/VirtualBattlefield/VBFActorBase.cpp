// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFActorBase.h"

#include "VBFUnitBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVBFActorBase::AVBFActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVBFActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVBFActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AVBFActorBase* AVBFActorBase::StaticCreateObject(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FVector& location, const FRotator& rotation)
{
	//nullptr is allowed while actor_class not parent of AVBFActorBase is not
	if (actor_class != nullptr 
		&& UKismetMathLibrary::ClassIsChildOf(actor_class, AVBFActorBase::StaticClass()) == false)
	{
		return nullptr;
	}

	//Deffered spawn is used to set ItemObject
	AVBFActorBase* spawned_actor = world->SpawnActorDeferred<AVBFActorBase>(actor_class, { rotation, location }, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

	if (spawned_actor == nullptr)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObject: SpawnActorDeferred failed"));
		return nullptr;
	}

	if (IsValid(unit_object) == false)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObject: Invalid ItemObject"));
		spawned_actor->Destroy();
		return nullptr;
	}

	spawned_actor->SetVBFUnit(unit_object);

	UGameplayStatics::FinishSpawningActor(spawned_actor, { rotation, location });

	if (IsValid(spawned_actor) == false)
	{
		return nullptr;
	}

	//TODO: test ownership of UVBFUnitBase
	//unit_object->Rename(nullptr, world);

	return spawned_actor;
}

