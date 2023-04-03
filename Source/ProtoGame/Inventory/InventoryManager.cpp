// Nikita Belov, All rights reserved


#include "Inventory/InventoryManager.h"

UInventoryManager::UInventoryManager()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UInventoryManager::BeginPlay()
{
	Super::BeginPlay();
}


void UInventoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

