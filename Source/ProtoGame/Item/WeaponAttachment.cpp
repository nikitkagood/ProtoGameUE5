// Nikita Belov, All rights reserved


#include "Item/WeaponAttachment.h"
#include "Item/WeaponBase.h"

// Sets default values for this component's properties
UWeaponAttachment::UWeaponAttachment()
{
	//PrimaryComponentTick.bCanEverTick = false;
	//PrimaryComponentTick.bStartWithTickEnabled = false;

	//SetEnableGravity(false);
	//SetSimulatePhysics(false);
	//CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	//SetCollisionProfileName("IgnorePawnVehicle_BlockElse");
}


bool UWeaponAttachment::SetProperties(FDataTableRowHandle handle)
{
	checkf(false, TEXT("UWeaponAttachment is an abstract class. Setting it's properties isn't meaningful."));

	return false;
}

void UWeaponAttachment::SetOuterWeapon(UWeaponBase* weapon)
{
	this->Rename(nullptr, weapon);
}

UWeaponBase* UWeaponAttachment::GetOuterWeapon() const
{
	return Cast<UWeaponBase>(GetOuter());
}

FName UWeaponAttachment::GetSocketName()
{
	if(attachment_info.slot.slot_number > 0)
	{
		return FName(attachment_info.socket_name.ToString() + '_' + FString::FromInt(attachment_info.slot.slot_number));
	}
	else
	{
		return attachment_info.socket_name;
	}
}

bool UWeaponAttachment::isAttachmentCompatible(FAttachmentSlot slot) const
{
	//TODO: Temporary version
	bool result = slot.slot_class == attachment_info.slot.slot_class && slot.slot_type == attachment_info.slot.slot_type;

	return result;
}

//void UWeaponAttachment::BeginPlay()
//{
//	Super::BeginPlay();
//}

//void UWeaponAttachment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

