// Nikita Belov, All rights reserved

#include "ItemActor.h"
#include "ItemBase.h"

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
		return nullptr;
	}

	spawned_actor->SetItemObject(item_object);

	if (item_object == nullptr)
	{
		//Although ItemActors can spawn their own ItemObjects, 
		//StaticCreateObject is meant to be used primarily with inventory and has to avoid "duping", 
		//i.e. creating new items out of thin air
		//This behaviour may change
		checkf(false, TEXT("Error: Invalid ItemObject"))
	}
	else
	{
		item_object->SetOuterItemActor(spawned_actor);
	}

	UGameplayStatics::FinishSpawningActor(spawned_actor, { rotation, location });

	if (spawned_actor->IsPendingKill())
	{
		return nullptr;
	}

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
		StaticMeshComp->SetCustomDepthStencilValue(static_cast<uint8>(InteractionType::Item));

	}
	else
	{
		SkeletalMeshComp->GetChildrenComponents(true, children);

		SkeletalMeshComp->SetRenderCustomDepth(true);
		SkeletalMeshComp->SetCustomDepthStencilValue(static_cast<uint8>(InteractionType::Item));
	}

	FLatentActionInfo latent_action(0, GetUniqueID(), TEXT("StopDrawingOutline"), this);
	UKismetSystemLibrary::RetriggerableDelay(GetWorld(), 0.05f, latent_action);

	for (auto& i : children)
	{
		auto child_primitive = Cast<UPrimitiveComponent>(i);
		child_primitive->SetRenderCustomDepth(true);
		child_primitive->SetCustomDepthStencilValue(static_cast<uint8>(InteractionType::Item));
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

void AItemActor::AttachItemActorToComponent(USceneComponent* Parent, EAttachmentRule AttachmentRule, bool weld, FName SocketName)
{
	bInteractable = false;

	if(GetSkeletalMeshComp() != nullptr)
	{
		GetSkeletalMeshComp()->SetSimulatePhysics(false);
	}
	if(GetStaticMeshComp() != nullptr)
	{
		GetStaticMeshComp()->SetSimulatePhysics(false);
	}

	AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);

	if(item_actor_mesh_type == ItemActorMeshType::StaticMesh)
	{
		GetStaticMeshComp()->AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);
	}
	else //skeletal mesh
	{
		GetSkeletalMeshComp()->AttachToComponent(Parent, { AttachmentRule, weld }, SocketName);
	}
}

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

bool AItemActor::OnInteract_Implementation(AActor* caller)
{
	if(ItemObject == nullptr)
	{
		check(false);
        UE_LOG(LogTemp, Fatal, TEXT("ItemObject for ItemActor: %s is invalid!"), *GetName());
		
		return false;
	}

	//Ask ItemObject what to do
	//This behaviour might be reduntant and may change later
	if(bInteractable == true && ItemObject->Interact(caller) == true)
	{
		return Destroy(); //currently ItemActors are obliged to destroy themselves
	}
	else
	{
		return false;
	}

}


