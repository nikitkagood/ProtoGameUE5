// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFActorBase.h"

#include "VBFUnitBase.h"
//#include "Components/PawnMovement.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AVBFActorBase::AVBFActorBase()
{
	//SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent")));

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollisionComponent"));
	//CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	//CapsuleComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	//CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	//CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	//CapsuleComponent->SetCanEverAffectNavigation(false);
	//CapsuleComponent->bDynamicObstacle = true;

	//SetRootComponent(CapsuleComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AVBFActorBase* AVBFActorBase::StaticCreateObjectDeferred(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FTransform& transform, ESpawnActorCollisionHandlingMethod collision_handling)
{
	//nullptr is allowed while actor_class not parent of AVBFActorBase is not
	if (actor_class != nullptr
		&& UKismetMathLibrary::ClassIsChildOf(actor_class, AVBFActorBase::StaticClass()) == false)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObjectDeferred: not a valid class for Actor"));
		return nullptr;
	}

	if (IsValid(unit_object) == false)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObjectDeferred: Invalid ItemObject"));
		return nullptr;
	}

	AVBFActorBase* spawned_actor = world->SpawnActorDeferred<AVBFActorBase>(actor_class, transform, nullptr, nullptr, collision_handling);

	if (spawned_actor == nullptr)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObjectDeferred: SpawnActorDeferred failed"));
		return nullptr;
	}

	spawned_actor->SetVBFUnit(unit_object);

	//FVector closest_point;
	//auto distance_to_collision = spawned_actor->ActorGetDistanceToCollision(transform.GetLocation(), ECollisionChannel::ECC_WorldStatic, closest_point);
	//UE_LOG(LogTemp, Warning, TEXT("ActorGetDistanceToCollision: %d, closest point: %s"), distance_to_collision, *closest_point.ToString());

	//TODO: test ownership of UVBFUnitBase
	//unit_object->Rename(nullptr, world);

	return spawned_actor;
}


// Called when the game starts or when spawned
void AVBFActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

//bool AVBFActorBase::SetupMeshComponent(UStreamableRenderAsset* render_asset)
//{
//	if (render_asset == nullptr)
//	{
//		return false;
//	}
//
//	if (IsValid(MeshComponent))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("AVBFActorBase::SetupMeshComponent: MeshComponent is set already"))
//			return false;
//	}
//
//	if (render_asset->GetClass() == UStaticMesh::StaticClass())
//	{
//		MeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("StaticMeshComp"));
//
//		auto* StaticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
//		StaticMeshComp->SetStaticMesh(Cast<UStaticMesh>(render_asset));
//
//		MeshComponent->SetEnableGravity(false);
//		MeshComponent->SetSimulatePhysics(false);
//		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
//		MeshComponent->SetGenerateOverlapEvents(true);
//		MeshComponent->SetCollisionProfileName("BlockAllDynamic");
//	}
//	else if (render_asset->GetClass() == USkeletalMesh::StaticClass())
//	{
//		MeshComponent = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkeletalMeshComp"));
//
//		auto* SkeletalMeshComp = Cast<USkeletalMeshComponent>(MeshComponent);
//
//		SkeletalMeshComp->SetSkeletalMeshAsset(Cast<USkeletalMesh>(render_asset));
//		SkeletalMeshComp->bCollideWithAttachedChildren = false;
//
//		MeshComponent->SetEnableGravity(false);
//		MeshComponent->SetSimulatePhysics(false);
//		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
//		MeshComponent->SetGenerateOverlapEvents(true);
//		MeshComponent->SetCollisionProfileName("BlockAllDynamic");
//	}
//	else
//	{
//		return false;
//	}
//
//	//UE_LOG(LogTemp, Warning, TEXT("TEST: RelativeTransform: %s"), *GetRootComponent()->GetRelativeTransform().ToString())
//	//MeshComponent->SetupAttachment(GetRootComponent(), {});
//	if (GetRootComponent() != nullptr)
//	{
//		MeshComponent->SetRelativeTransform(GetRootComponent()->GetRelativeTransform());
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("AVBFActorBase::SetupMeshComponent: failed to set relative transform: Root is null"));
//	}
//
//	//FAttachmentTransformRules rules { EAttachmentRule::SnapToTarget, true };
//	//MeshComponent->AttachToComponent(GetRootComponent(), rules);
//	MeshComponent->RegisterComponent();
//
//	return true;
//}

// Called every frame
void AVBFActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//bool AVBFActorBase::Initialize(UStreamableRenderAsset* render_asset)
//{
//	//return SetupMeshComponent(render_asset);
//	return true;
//}

TScriptInterface<IVBFUnitInterface> AVBFActorBase::GetVBFUnitInterface(bool& is_valid)
{
	if (IsValid(vbf_unit) == false)
	{
		is_valid = false;
		return nullptr;
	}

	is_valid = true;

	return TScriptInterface<IVBFUnitInterface>(vbf_unit);
}

//void AVBFActorBase::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//
//	if (IsValid(this))
//	{
//		//if (Mesh)
//		//{
//		//	CacheInitialMeshOffset(Mesh->GetRelativeLocation(), Mesh->GetRelativeRotation());
//
//		//	// force animation tick after movement component updates
//		//	if (Mesh->PrimaryComponentTick.bCanEverTick && CharacterMovement)
//		//	{
//		//		Mesh->PrimaryComponentTick.AddPrerequisite(CharacterMovement, CharacterMovement->PrimaryComponentTick);
//		//	}
//		//}
//
//		if (MovementComponent && CapsuleComponent)
//		{
//			MovementComponent->UpdateNavAgent(*CapsuleComponent);
//		}
//
//		if (Controller == nullptr && GetNetMode() != NM_Client)
//		{
//			if (MovementComponent && MovementComponent->bRunPhysicsWithNoController)
//			{
//				MovementComponent->SetDefaultMovementMode();
//			}
//		}
//	}
//}

