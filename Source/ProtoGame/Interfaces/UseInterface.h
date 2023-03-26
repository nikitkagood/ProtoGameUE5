// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UseInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UUseInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOGAME_API IUseInterface
{
	GENERATED_BODY()

public:
	virtual bool OnUse(AActor* caller) = 0; //to be overriden in C++ classes that implement this interface
};
