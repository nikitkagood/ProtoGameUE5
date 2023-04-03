// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryComponent.h"

#include "InventoryManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOGAME_API UInventoryManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<IInventoryInterface*> inventories;
};
