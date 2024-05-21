// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "VBFUnitBase.h"

#include "VBFMap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapUpdated);

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API UVBFMap : public UObject
{
	GENERATED_BODY()
	
public:
	UVBFMap();

	UTexture2D* GetHeightMap() { return height_map; }
	const UTexture2D* GetHeightMap() const { return height_map; }

	UFUNCTION(BlueprintCallable)
	const TArray<UVBFUnitBase*> GetUnitsOnMap() const { return UnitsOnMap; }

	virtual void UpdateMap() { OnMapUpdated.Broadcast(); };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	TArray<UVBFUnitBase*> UnitsOnMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear, meta = (AllowPrivateAccess = true))
	UTexture2D* height_map;
	//TArray<TArray<FFloat16>> height_map;

	//value represents forest density
	TArray<TArray<uint8>> forest_map;

	//value represents water depth
	TArray<TArray<uint8>> water_map;

	//TArray<TArray<???>> roads_map;
	//TArray<TArray<???>> terrain_feature_map;

	UPROPERTY(BlueprintAssignable)
	FOnMapUpdated OnMapUpdated;
};
