// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "EffectBase.generated.h"

//Tiny base class for effects which are AActor in nature (such as lights). This class is mainly used to avoid TSubClassOf<AActor> references.
UCLASS(BlueprintType)
class PROTOGAME_API AEffectBase : public AActor
{
	GENERATED_BODY()
	
public:	

};
