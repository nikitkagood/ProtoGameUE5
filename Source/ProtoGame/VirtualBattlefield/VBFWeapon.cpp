// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFWeapon.h"

#include "Interfaces/VBFUnitInterface.h"

UObject* UVBFWeapon::GetWeaponOwner_Implementation() const
{
    return GetOuter();
}

bool UVBFWeapon::SetWeaponActive_Implementation(bool new_active) const
{
    return vbf_weapon_info.IsActive;
}

bool UVBFWeapon::CanWeaponAttack_Implementation() const
{
    return IsWeaponActive() && vbf_weapon_info.AmmoLeft > 1;
}

double UVBFWeapon::GetWeaponHitChance_Implementation(double target_distance, const FVector& target_location, const FVector& target_size, double target_speed) const
{
    return 0.0;
}

bool UVBFWeapon::WeaponTarget_Implementation(const UObject* target)
{
    if (IsValid(target) == false)
    {
        return false;
    }

    //By default it makes sense to target VBFUnits only
    if (target->Implements<UVBFUnitInterface>() == false)
    {
        return false;
    }

    //auto unit = Cast<IVBFUnitInterface>(target);

    vbf_weapon_info.Target = target;

    return true;
}

bool UVBFWeapon::WeaponTargetLocation_Implementation(FVector location)
{
    return false;
}

bool UVBFWeapon::WeaponAttack_Implementation()
{
    //Check target

    if (WeaponUseAmmo())
    {

    }

    //Apply damage

    return true;
}

int64 UVBFWeapon::WeaponUseAmmo_Implementation(int64 count, bool strict_check)
{
    if (count < 1 || GetWeaponAmmoMax() < 1)
    {
        return 0;
    }

    if (vbf_weapon_info.AmmoLeft < count)
    {
        if (strict_check == true)
        {
            return 0;
        }
        else
        {
            int64 remaining_ammo_used = vbf_weapon_info.AmmoLeft;
            vbf_weapon_info.AmmoLeft = 0;

            return remaining_ammo_used;
        }
    }

    vbf_weapon_info.AmmoLeft -= count;
    return count;
}

int64 UVBFWeapon::WeaponAddAmmo_Implementation(int64 count, bool strict_check)
{
    if (count < 1 || GetWeaponAmmoMax() < 1)
    {
        return 0;
    }

    if ((vbf_weapon_info.AmmoLeft + count) > GetWeaponAmmoMax())
    {
        if (strict_check == true)
        {
            return 0;
        }
        else
        {
            int64 ammo_added_until_max = GetWeaponAmmoMax() - vbf_weapon_info.AmmoLeft;
            vbf_weapon_info.AmmoLeft = GetWeaponAmmoMax();

            return ammo_added_until_max;
        }
    }

    vbf_weapon_info.AmmoLeft += count;
    return count;
}
