// Nikita Belov, All rights reserved


#include "VirtualBattlefield/VBFActorBase.h"

#include "VBFUnitBase.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVBFActorBase::AVBFActorBase()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent")));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

AVBFActorBase* AVBFActorBase::StaticCreateObject(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FTransform& transform)
{
	//nullptr is allowed while actor_class not parent of AVBFActorBase is not
	if (actor_class != nullptr
		&& UKismetMathLibrary::ClassIsChildOf(actor_class, AVBFActorBase::StaticClass()) == false)
	{
		return nullptr;
	}

	if (IsValid(unit_object) == false)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObject: Invalid ItemObject"));
		return nullptr;
	}

	AVBFActorBase* spawned_actor = world->SpawnActorDeferred<AVBFActorBase>(actor_class, transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (spawned_actor == nullptr)
	{
		checkf(false, TEXT("Error: AVBFActorBase::StaticCreateObject: SpawnActorDeferred failed"));
		return nullptr;
	}

	spawned_actor->SetVBFUnit(unit_object);

	UGameplayStatics::FinishSpawningActor(spawned_actor, transform);

	if (IsValid(spawned_actor) == false)
	{
		return nullptr;
	}


	//TODO: test ownership of UVBFUnitBase
	//unit_object->Rename(nullptr, world);

	return spawned_actor;
}


// Called when the game starts or when spawned
void AVBFActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AVBFActorBase::SetupMeshComponent(UStreamableRenderAsset* render_asset)
{
	if (render_asset == nullptr)
	{
		return false;
	}

	if (IsValid(MeshComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("AVBFActorBase::SetupMeshComponent: MeshComponent is set already"))
		return false;
	}

	if (render_asset->GetClass() == UStaticMesh::StaticClass())
	{
		MeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("StaticMeshComp"));

		auto* StaticMeshComp = Cast<UStaticMeshComponent>(MeshComponent);
		StaticMeshComp->SetStaticMesh(Cast<UStaticMesh>(render_asset));

		MeshComponent->SetEnableGravity(false);
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->SetCollisionProfileName("BlockAllDynamic");
	}
	else if (render_asset->GetClass() == USkeletalMesh::StaticClass())
	{
		MeshComponent = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkeletalMeshComp"));

		auto* SkeletalMeshComp = Cast<USkeletalMeshComponent>(MeshComponent);

		SkeletalMeshComp->SetSkeletalMeshAsset(Cast<USkeletalMesh>(render_asset));
		SkeletalMeshComp->bCollideWithAttachedChildren = false;

		MeshComponent->SetEnableGravity(false);
		MeshComponent->SetSimulatePhysics(false);
		MeshComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->SetCollisionProfileName("BlockAllDynamic");
	}
	else
	{
		return false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("TEST: RelativeTransform: %s"), *GetRootComponent()->GetRelativeTransform().ToString())
	//MeshComponent->SetupAttachment(GetRootComponent(), {});
	MeshComponent->SetRelativeTransform(GetRootComponent()->GetRelativeTransform());
	SetRootComponent(MeshComponent);
	MeshComponent->RegisterComponent();

	return true;
}

// Called every frame
void AVBFActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AVBFActorBase::Initialize(UStreamableRenderAsset* render_asset)
{
	return SetupMeshComponent(render_asset);
}

