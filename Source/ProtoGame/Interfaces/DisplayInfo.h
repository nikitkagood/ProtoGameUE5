// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Templates/Tuple.h"

#include "DisplayInfo.generated.h"

USTRUCT(BlueprintType)
struct FDisplayInfoPayload // : public FTableRowBase
{
	GENERATED_BODY()

	//Thing to display that is not a text
	//Leave empty if you want simply text
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UObject* ObjectPtr = nullptr;

	//Text to display
	//If ObjectPtr is valid, then it's interpreted as *object* + text
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FText Text;

	//Where to place information
	//Can be interpreted differently
	//By default it's: Row, Column in a grid
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<double> Layout;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDisplayInfo : public UInterface
{
	GENERATED_BODY()
};

//Get info about some game object that is to be displayed in UI
class PROTOGAME_API IDisplayInfo
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FDisplayInfoPayload> GetDisplayInfo();
	//void GetDisplayInfo(TArray<FText>& OutTextArr, TArray<UObject*>& OutObjectArr);

	//Default example implementation, has to be overriden
	TArray<FDisplayInfoPayload> GetDisplayInfo_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FName> GetSupportedClassNames();

	//Example. UCurveFloat and UTexture2D
	TArray<FName> GetSupportedClassNames_Implementation();
};
