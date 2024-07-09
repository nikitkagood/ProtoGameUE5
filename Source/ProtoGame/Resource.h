// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Resource.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	Money = 1				UMETA(DisplayName = "Money"),
	Manpower = 2			UMETA(DisplayName = "Manpower"),
};


USTRUCT(BlueprintType)
struct PROTOGAME_API FResource : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = -1))
	EResourceType type;
};
