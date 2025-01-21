// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Interfaces/VBFWeaponInterface.h"

#include "VBFWeapon.generated.h"

//A wrapper around TArray
USTRUCT(BlueprintType)
struct PROTOGAME_API FVBFWeaponsArray
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<UVBFWeapon*> Weapons;
};


USTRUCT(BlueprintType)
struct FVBFWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Name = "DefaultWeaponName";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EVBFWeaponTypes WeaponTypeEnum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsActive = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FVBFDamageInfo> DamageInfo;

	//Projectile spread in milliradians (mills)
	//The less the better
	//1 mil at 100m = 10cm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	double AccuracyMilliradians = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	double InitialVelocity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int64 AmmoMax = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int64 AmmoLeft = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const UObject* Target;
};

USTRUCT(BlueprintType)
struct FVBFWeaponInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int64 AmmoMax;

	FVBFWeaponInfo vbf_weapon_info;

};

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, ClassGroup = VirtualBattlefield)
class PROTOGAME_API UVBFWeapon : public UObject, public IVBFWeaponInterface
{
	GENERATED_BODY()
public:
	//double GetOwnerLocation();
	//double GetOwnLocation();
	//double GetOwnerSpeed();
	//double GetOwnSpeed();
	//double GetOwnerRotation();
	//double GetOwnRotation();

	//bool IsTargetValid();

	//IVBFWeaponInterface

	FName GetWeaponName_Implementation() const { return vbf_weapon_info.Name; };

	UObject* GetWeaponOwner_Implementation() const;

	EVBFWeaponTypes GetWeaponType_Implementation(uint8& WeaponType) const { WeaponType = vbf_weapon_info.WeaponType; return vbf_weapon_info.WeaponTypeEnum; };

	bool SetWeaponActive_Implementation(bool new_active) const;

	bool IsWeaponActive_Implementation() const { return vbf_weapon_info.IsActive; };

	bool CanWeaponAttack_Implementation() const;

	TArray<FVBFDamageInfo> GetWeaponDamageInfo_Implementation() const { return vbf_weapon_info.DamageInfo;	};

	const UObject* GetTarget_Implementation() const { return vbf_weapon_info.Target; };

	//2000 meters is arbitrary default value
	double GetWeaponMaxRangeLimit_Implementation() const { return 2000 * 100; };

	//1500 meters is arbitrary default value
	double GetWeaponMaxRange_Implementation() const { return 1500 * 100; };

	double GetWeaponMinRange_Implementation() const { return 0; };

	//TArray<double> GetWeaponEffectiveRanges() const

	//EWeaponAccuracyType GetWeaponAccuracyType() const

	double GetWeaponAbsoluteAccuracyMils_Implementation() const { return vbf_weapon_info.AccuracyMilliradians; };

	//TODO
	double GetWeaponHitChance_Implementation(double target_distance, const FVector& target_location, const FVector& target_size, double target_speed) const;

	double GetWeaponInitialVelocity_Implementation() const { return vbf_weapon_info.InitialVelocity; };

	int64 GetWeaponAmmoMax_Implementation() const { return vbf_weapon_info.AmmoMax; };

	int64 GetWeaponAmmoLeftOverall_Implementation() const { return vbf_weapon_info.AmmoLeft; };

	int64 GetWeaponAmmoCurrentMagazineCapacity_Implementation() const { return 0; };

	int64 GetWeaponAmmoLeftMagazine_Implementation() const { return 0; };

	bool WeaponTarget_Implementation(const UObject* target);

	bool WeaponTargetLocation_Implementation(FVector location);

	bool WeaponAttack_Implementation();

	int64 WeaponUseAmmo_Implementation(int64 count = 1, bool strict_check = false);

	int64 WeaponAddAmmo_Implementation(int64 count = 1, bool strict_check = false);

	//IVBFWeaponInterface end

	//Convert mils to MOA
	//1 MOA at 100m = 2,9089cm
	UFUNCTION(BlueprintCallable)
	static double MilsToMOA(double mils) { return mils * 3, 4377; };

	UFUNCTION(BlueprintCallable)
	static double MOAToMils(double MOA) { return MOA / 3, 4377; };
private:
	FVBFWeaponInfo vbf_weapon_info;
};
