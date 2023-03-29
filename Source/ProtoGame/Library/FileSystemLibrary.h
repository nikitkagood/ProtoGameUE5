// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileSystemLibrary.generated.h"

UCLASS()
class PROTOGAME_API UFileSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static bool IsTextureSafeToRead(UTexture* Texture);
};
