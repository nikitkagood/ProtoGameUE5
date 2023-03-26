// Nikita Belov, All rights reserved


#include "Item/WeaponBase.h"
#include "Item/Projectile.h"
#include "Character/GameCharacter.h"
#include "ItemActor.h"
#include "Item/WeaponAttachment.h"
#include "Item/WeaponAttachmentMagazine.h"

#include "Kismet/GameplayStatics.h"


void UWeaponBase::DestroySKStatic(USkeletalMeshComponent* sk_comp)
{
	if(sk_comp == nullptr)
	{
		return;
	}

	TArray<USceneComponent*> children;
	sk_comp->GetChildrenComponents(true, children);

	for(auto& i : children)
	{
		//sometimes this check is actually needed
		if(i != nullptr)
		{
			i->DestroyComponent();
		}
	}

	sk_comp->DestroyComponent();
}

//void UWeaponBase::SetWeaponInfo(FWeaponInfo&& value)
//{
//	weapon_info = value;
//}
