// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "VBFWeaponInterface.generated.h"

//Default weapon types

//Default types with their provided functionality
//Custom is to provide your own logic
UENUM(BlueprintType)
enum class EVBFWeaponTypes : uint8
{
	None = 0,
	EVBFWeaponInfantryFirearm,
	EVBFWeaponInfantryLauncher,
	EVBFWeaponArtillery,

	Custom,
};

//Default firearms
UENUM(BlueprintType)
enum class EVBFWeaponInfantryFirearm : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	AssaultRifle,			
	Carbine, //shorter assault rifle	
	SemiAutoRifle,							
	SMG,
	BoltActionRifle,
	LightMachineGun, //SAW for example
	MediumMachineGun, //M240B for example
};

UENUM(BlueprintType)
enum class EVBFWeaponInfantryLauncher : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	MultipleUseRPG,
	SingleUseRPG,
	ATGM,
};


UENUM(BlueprintType)
enum class EVBFWeaponArtillery : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	Cannon, //direct-fire field guns
	Howitzer,				
	Mortar,
	MLRS,
};


UENUM(BlueprintType)
enum class EVBFDamageType : uint8
{
	None = 0					UMETA(DisplayName = "None"),
	Pierce,
	Blunt,
	HE,
	HEAT,
	Thermal,
	Chemical,
	Electrical,

	//Some other logic that bypasses provided implementations
	Custom,
};

UENUM(BlueprintType)
enum class EWeaponAccuracyType : uint8
{
	None = 0,
	//Just in case
	Constant,					
	//Degrees, thus accuracy just linearly decreases
	Linear,			
	//Accuracy changes based on distance and controlled via curve
	Curve	UMETA(DisplayName = "Curve"),

	//Some other logic that bypasses provided implementations
	Custom,
};

//Most required information for damage calculations
//Velocity/energy is separate cause it's not always used
USTRUCT(BlueprintType)
struct FVBFDamageInfo // : public FTableRowBase
{
	GENERATED_BODY()

	EVBFDamageType type;
	float damage;
	float penetration;
};

//UENUM(BlueprintType)
//enum class EVBFVelocityType : uint8
//{
//	None = 0,
//	Constant,
//	Linear,
//	AirDrag,
//};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVBFWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOGAME_API IVBFWeaponInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FName GetWeaponName() const;

	//Who owns the weapon and has non weapon specific information
	//Doesn't have to be the Owner in Unreal sense (UObject::GetOwner())
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UObject* GetWeaponOwner() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SetWeaponOwner(UObject* new_owner);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UPARAM(DisplayName = "WeaponTypeEnum") EVBFWeaponTypes GetWeaponType(uint8& WeaponType) const;

	//return value: set value, != new_active means not successfull
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SetWeaponActive(bool new_active) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsWeaponActive() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanWeaponAttack() const;

	//TODO: unnecessary for now
	//-uint8: value of Enum; Byte to Enum then Select
	//-OutEnumName to define what Enum is actually used
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//uint8 GetWeaponTypeCustom(FString& OutEnumName) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FVBFDamageInfo> GetWeaponDamageInfo() const;

	//It's up to implementation to provide a meaningful target
	//By default it's IVBFUnitInterface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	const UObject* GetTarget() const;

	//Absolute limit for a "projectile"
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponMaxRangeLimit() const;

	//Cannot aqquire target past
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponMaxRange() const;

	//Cannot aqquire target before
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponMinRange() const;

	//When it actually makes sense to fire
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<double> GetWeaponEffectiveRanges() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EWeaponAccuracyType GetWeaponAccuracyType() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponAccuracyAtMinRange() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponAccuracyAtMaxRange() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponAccuracyAtDistance(double distance) const;

	//In milliradians (mills)
	//1 mil at 100m = 10cm
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponAbsoluteAccuracyMils() const;

	//Convert mils to MOA
	//1 MOA at 100m = 2,9089cm
	//UFUNCTION(BlueprintCallable)
	//double MilsToMOA(double mils) { return mils * 3, 4377; };

	//UFUNCTION(BlueprintCallable)
	//double MOAToMils(double MOA) { return MOA / 3, 4377; };

	//Hit chance of weapon itself, unaffected by anything else
	//This is an approximation. Actually firing any projectiles against complex shapes is different.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponHitChance(double target_distance, const FVector& target_location, const FVector& target_size, double target_speed) const;

	//Final hit chance
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponCompoundHitChance(double weapon_hit_chance, const TArray<double>& other_chances) const;

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//bool WeaponTargetCustom(UObject* target) const;

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, CustomThunk, meta = (DisplayName = "test function3", Keywords = "testkeyword", MapParam = "Options", MapKeyParam = "Output", AutoCreateRefTerm = "Options"), Category = "MyProject")
	//void TestFunction3(const TMap<int32, int32>& Options, int32& Output);
	
	//DECLARE_FUNCTION(execTestFunction3)
	//{
	//	// add some code to get params
	//}

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//double GetWeaponVelocityType() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	double GetWeaponInitialVelocity() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 GetWeaponAmmoMax() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 GetWeaponAmmoLeftOverall() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 GetWeaponAmmoCurrentMagazineCapacity() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 GetWeaponAmmoLeftMagazine() const;

	//So you may want to have more than just AOE area
	//Array index is used to indicate what AOE starting from the center one
	//Falloff: linear, non-linear
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//double GetWeaponAOE(TArray<double> );

	//It's up to implementation to provide a meaningful target
	//By default it's IVBFUnitInterface
	//Return value: whether target was accquired or not
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool WeaponTarget(const UObject* target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool WeaponTargetLocation(FVector location);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool WeaponAttack();

	//return value: ammo that was used
	//strict_check: 
	// -false: available ammo will be used 
	// -true: if not enough, no ammo used
	//count can't be < 1
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 WeaponUseAmmo(int64 count = 1, bool strict_check = false);

	//return value: ammo that was added
	//strict_check: 
	// -false: ammo will be added until max
	// -true: ammo won't be added at all if exceeds max
	//count can't be < 1
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int64 WeaponAddAmmo(int64 count = 1, bool strict_check = false);

	//Properties outside of usual ones that may or may not belong to a weapon
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//TArray<FString> GetWeaponTagsStr();
};

