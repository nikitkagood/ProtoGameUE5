// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"

#include "VBFUnitInfo.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UTexture2D;

USTRUCT(Blueprintable, BlueprintType)
struct PROTOGAME_API FVBFUnitInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FText Name = FText::FromString("DefaultUnitName");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool IsEverCommandable = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FRotator Rotation;

	//In UE units: m/s
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	double Speed;

	//In UE units: m/s
	//MaxSpeed of 0 is used to indicate that unit cannot move at all
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ClampMin = 0, UIMax = 1000))
	double MaxSpeed;

	//Things cannot occupy the same space so we have to account for it
	//Also may be useful for other things like accuracy calculations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, ClampMin = 0.01, UIMax = 5000))
	FVector Size{ 1., 1., 1. };
};

		
UENUM(BlueprintType)
enum class EUnitMeshType : uint8
{
	None = 0				UMETA(DisplayName = "None"),
	//Explicitly state that mesh logic comes from somewhere else
	UseExternalSettings 	UMETA(DisplayName = "Use External Settings"),
	UseStaticMesh			UMETA(DisplayName = "Use Static Mesh"),
	UseSkeletalMesh			UMETA(DisplayName = "Use Skeletal Mesh"),
};

USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFUnitInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	FVBFUnitInfoTable();

	//Mandatory. Used in ConstructVBFUnit.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
	TSubclassOf<class UVBFUnitBase> UnitClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, meta = (AllowPrivateAccess = true))
	TSubclassOf<class AVBFActorBase> UnitActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EUnitMeshType MeshType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USkeletalMesh* SkeletalMesh;



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FVBFUnitInfo unit_info;
};