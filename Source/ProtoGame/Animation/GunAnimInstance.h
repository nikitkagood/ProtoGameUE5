// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "GunAnimInstance.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateAnimInstance);

class UWeaponGun;
enum class WeaponFireMode : uint8;

UCLASS()
class PROTOGAME_API UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetWeaponGun(UWeaponGun* Gun) { WeaponGun = Gun; };
	void SetFireMode(WeaponFireMode Mode) { FireMode = Mode; };

	//UPROPERTY(BlueprintAssignable)
	//FUpdateAnimInstance OnAnimInstanceUpdated;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, NoClear)
	UWeaponGun* WeaponGun;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, NoClear)
	WeaponFireMode FireMode;
};
