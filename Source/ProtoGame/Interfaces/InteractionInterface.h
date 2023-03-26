// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InteractionInterface.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UInteractionInterface : public UInterface
{
	//has to be empty
	GENERATED_BODY()
};

class PROTOGAME_API IInteractionInterface
{
	GENERATED_BODY()

public:
	virtual bool OnInteract(AActor* caller) = 0; //to be overriden in C++ classes that implement this interface
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interaction") //to be implemented in BP
	bool OnInteractBP(AActor* caller);
};
