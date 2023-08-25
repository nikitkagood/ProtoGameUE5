// Nikita Belov, All rights reserved

#include "ItemActor.h"
#include "ItemBase.h"

#include "InventoryItem.h"

#include "Kismet/GameplayStatics.h"

AItemActor::AItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SceneComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	if(item_actor_mesh_type == ItemActorMeshType::StaticMesh || GetDefault<AItemActor>() == this)
	{
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
		StaticMeshComp->SetupAttachment(SceneComponent, {});

		StaticMeshComp->SetEnableGravity(true);
		StaticMeshComp->SetSimulatePhysics(true);
		StaticMeshComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		StaticMeshComp->SetGenerateOverlapEvents(true);
		StaticMeshComp->SetCollisionProfileName("Item");
	}
	if(item_actor_mesh_type == ItemActorMeshType::SkeletalMesh || GetDefault<AItemActor>() == this)
	{
		SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
		SkeletalMeshComp->SetupAttachment(SceneComponent, {});

		SkeletalMeshComp->bCollideWithAttachedChildren = false;

		SkeletalMeshComp->SetEnableGravity(true);
		SkeletalMeshComp->SetSimulatePhysics(true);
		SkeletalMeshComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		StaticMeshComp->SetGenerateOverlapEvents(true);
		SkeletalMeshComp->SetCollisionProfileName("Item");
	}

	SetRootComponent(SceneComponent);

	ItemObject = nullptr;
	ItemBaseClass = UItemBase::StaticClass();

	item_object_creation_method = ItemObjectCreationMethod::CreateItemObjectFromDataTable;

	bInteractable = true;
}

AItemActor* AItemActor::StaticCreateObject(UWorld* world, TSubclassOf<AItemActor> item_actor_class, UItemBase* item_object, const FVector& location, const FRotator& rotation)
{
	//Deffered spawn is used to set ItemObject
	AItemActor* spawned_actor = world->SpawnActorDeferred<AItemActor>(item_actor_class, { rotation, location }, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

	if(spawned_actor == nullptr)
	{
		checkf(false, TEXT("Error: AItemActor::StaticCreateObject: SpawnActorDeferred failed"));
		return nullptr;
	}

	if (IsValid(item_object) == false)
	{
		//Although ItemActors can spawn their own ItemObjects, 
		//StaticCreateObject is meant to be used primarily with inventory and has to avoid "duping", 
		//i.e. creating new items out of thin air
		//This behaviour may change
		checkf(false, TEXT("Error: AItemActor::StaticCreateObject: Invalid ItemObject"));
		spawned_actor->Destroy();
		return nullptr;
	}

	spawned_actor->SetItemObject(item_object);

	UGameplayStatics::FinishSpawningActor(spawned_actor, { rotation, location });

	if (IsValid(spawned_actor) == false)
	{
		return nullptr;
	}

	item_object->SetOuterItemActor(spawned_actor);

	return spawned_actor;
}

AItemActor* AItemActor::StaticCreateObjectVisualOnly(UWorld* world, TSubclassOf<AItemActor> item_actor_class, UItemBase* item_object, const FVector& location, const FRotator& rotation)
{
	AItemActor* spawned_actor = world->SpawnActorDeferred<AItemActor>(item_actor_class, { rotation, location }, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding);

	if(spawned_actor != nullptr)
	{
		spawned_actor->bInteractable = false;
		spawned_actor->SetItemObject(item_object);

		UGameplayStatics::FinishSpawningActor(spawned_actor, { rotation, location });
		return spawned_actor;
	}

	return nullptr;
}

void AItemActor::DrawInteractionOutline_Implementation()
{
	TArray<USceneComponent*> children;

	if (item_actor_mesh_type == ItemActorMeshType::StaticMesh || GetDefault<AItemActor>() == this)
	{
		StaticMeshComp->GetChildrenComponents(true, children);

		StaticMeshComp->SetRenderCustomDepth(true);
		StaticMeshComp->SetCustomDepthStencilValue(static_cast<uint8>(EInteractionType::Item));

	}
	else
	{
		SkeletalMeshComp->GetChildrenComponents(true, children);

		SkeletalMeshComp->SetRenderCustomDepth(true);
		SkeletalMeshComp->SetCustomDepthStencilValue(static_cast<uint8>(EInteractionType::Item));
	}

	FLatentActionInfo latent_action(0, GetUniqueID(), TEXT("StopDrawingOutline"), this);
	UKismetSystemLibrary::RetriggerableDelay(GetWorld(), 0.05f, latent_action);

	for (auto& i : children)
	{
		auto child_primitive = Cast<UPrimitiveComponent>(i);
		child_primitive->SetRenderCustomDepth(true);
		child_primitive->SetCustomDepthStencilValue(static_cast<uint8>(EInteractionType::Item));
	}
}

void AItemActor::StopDrawingOutline_Implementation()
{
	TArray<USceneComponent*> children;

	if (item_actor_mesh_type == ItemActorMeshType::StaticMesh || GetDefault<AItemActor>() == this)
	{
		StaticMeshComp->GetChildrenComponents(true, children);

		StaticMeshComp->SetRenderCustomDepth(false);
	}
	else
	{
		SkeletalMeshComp->GetChildrenComponents(true, children);

		SkeletalMeshComp->SetRenderCustomDepth(false);
	}

	for (auto& i : children)
	{
		Cast<UPrimitiveComponent>(i)->SetRenderCustomDepth(false);
	}
}

void AItemActor::SetAnimClass(TSubclassOf<UAnimInstance> anim_class)
{
	SkeletalMeshComp->SetAnimClass(anim_class);
}

//void AItemActor::AttachItemActorToComponent(USceneComponent* Parent, EAttachmentRule AttachmentRule, bool weld, FName SocketName)
//{
//	bInteractable = false;
//
//	if(GetSkeletalMeshComp() != nullptr)
//	{
//		GetSkeletalMeshComp()->SetSimulatePhysics(false);
//	}
//	if(GetStaticMeshComp() != nullptr)
//	{
//		GetStaticMeshComp()->SetSimulatePhysics(false);
//	}
//
//	AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);
//
//	if(item_actor_mesh_type == ItemActorMeshType::StaticMesh)
//	{
//		GetStaticMeshComp()->AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);
//	}
//	else //skeletal mesh
//	{
//		GetSkeletalMeshComp()->AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);
//	}
//}

//void AItemActor::TransferNonSceneComponentOwnership(AActor* new_owner)
//{
//	if (new_owner == nullptr)
//	{
//		checkf(false, TEXT("Error: TransferNonSceneComponentOwnership: new_owner is invalid"));
//		return;
//	}
//
//	if (new_owner == this)
//	{
//		checkf(false, TEXT("Owner is set to this already"))
//		return;
//	}
//
//	auto comps_copy = GetComponents();
//
//	for (auto* comp : comps_copy)
//	{
//		auto comp_owner = comp->GetOwner();
//
//		if (IsValid(comp) && comp->IsA<USceneComponent>() == false)
//		{
//			comp->Rename(nullptr, new_owner);
//			this->RemoveOwnedComponent(comp);
//			new_owner->AddOwnedComponent(comp);
//		}
//	}
//
//}

//bool AItemActor::DestroyAndMoveComps(AActor* new_comp_owner)
//{
//	//TransferNonSceneComponentOwnership(new_comp_owner);
//
//	return Destroy();
//}

void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if(ItemObject == nullptr)
	{
		CreateItemObject();
	}

	//Inactive mesh component gets destroyed
	//They are both created in the first place only for CDO and Editor
	if(item_actor_mesh_type == ItemActorMeshType::StaticMesh)
	{
		SkeletalMeshComp->DestroyComponent();
	}
	else if(item_actor_mesh_type == ItemActorMeshType::SkeletalMesh)
	{
		StaticMeshComp->DestroyComponent();
	}
}

void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemActor::CreateItemObject()
{	
	ItemObject = UItemBase::StaticCreateObject<UItemBase>(this, ItemBaseClass, item_object_creation_method, DT_ItemProperties);
}

bool AItemActor::OnInteract_Implementation(AActor* caller, EInteractionActions action)
{
	if(ItemObject == nullptr)
	{
		check(false);
        UE_LOG(LogTemp, Fatal, TEXT("ItemObject for ItemActor: %s is invalid!"), *GetName());
		
		return false;
	}

	if(bInteractable == true && ItemObject->Interact(caller, action) == true)
	{
		//auto comps_copy = GetComponents();

		//for (auto* comp : comps_copy)
		//{
		//	auto comp_owner = comp->GetOwner();

		//	if (IsValid(comp) && comp->IsA<USceneComponent>() == false)
		//	{
		//		RemoveOwnedComponent(comp);
		//		comp->Rename(nullptr, caller);
		//		caller->AddOwnedComponent(comp);
		//	}
		//}

		//auto item_obj_outer_before_destr = ItemObject->GetOuter();
		//auto item_obj_owner_before_destr = ItemObject->GetOwner();

		//UE_LOG(LogTemp, Warning, TEXT("Obj (before destroy): Outer: %s, Owner: %s"), *item_obj_outer_before_destr->GetName(), item_obj_owner_before_destr == nullptr ? TEXT("NONE") : *item_obj_owner_before_destr->GetName())

		//bool destroy_res = Destroy();

		//auto item_obj_outer = ItemObject->GetOuter();
		//auto item_obj_owner = ItemObject->GetOwner();
		//bool is_unreachable_obj = ItemObject->IsUnreachable();
		//bool is_valid_obj = IsValid(ItemObject);

		//UE_LOG(LogTemp, Warning, TEXT("Obj (after destroy): IsUnreachable: %i, IsValid: %i, Outer: %s, Owner: %s"), is_unreachable_obj, is_valid_obj, *item_obj_outer->GetName(), item_obj_owner == nullptr ? TEXT("NONE") : *item_obj_owner->GetName())
		//
		//auto inv_item = Cast<UInventoryItem>(ItemObject);
		//if (inv_item)
		//{
		//	auto inv_comp = inv_item->GetInventoryComponent();
		//	
		//	if (inv_comp)
		//	{
		//		bool is_unreachable = inv_comp->IsUnreachable();
		//		bool is_pend_kill = IsValid(inv_comp);

		//		UE_LOG(LogTemp, Warning, TEXT("InvComp: IsUnreachable: %i, IsValid: %i, Outer: %s, Owner: %s"), is_unreachable, is_pend_kill, *inv_comp->GetOuter()->GetName(), inv_comp->GetOwner() == nullptr ? TEXT("NONE") : *inv_comp->GetOwner()->GetName())
		//	}
		//}

		//return DestroyAndMoveComps();

		return Destroy(); //currently ItemActors are obliged to destroy themselves
	}
	else
	{
		return false;
	}

}


