// Nikita Belov, All rights reserved


#include "Components/InteractionManager.h"

#include "GameFramework/Controller.h"
#include "CollisionShape.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"

#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UInteractionManager::UInteractionManager()
{
	//don't tick, timer only
	PrimaryComponentTick.bCanEverTick = false;
	
}

// Called when the game starts
void UInteractionManager::BeginPlay()
{
	Super::BeginPlay();
	
	controller = Cast<APawn>(GetOwner())->GetController();

	CollisionParams.AddIgnoredActor(GetOwner());

	trace_delegate.BindUObject(this, &UInteractionManager::OnSweepCompletedAsync);

	const float rate = 1.f / 30.f; //30 FPS
	//GetWorld()->GetTimerManager().SetTimer(interaction_loop_timer_handle, this, &UInteractionManager::SweepInteractionFromView, rate, true, 0.f);
	GetWorld()->GetTimerManager().SetTimer(interaction_loop_timer_handle, this, &UInteractionManager::SweepInteractionFromViewAsync, rate, true, 0.f);
}

void UInteractionManager::GetInteractionStartPoint_Implementation(FVector& start_point, FRotator& start_rot)
{
	//Don't just use the arguments blindly,
	// they may not be initialized at all

	//Default: get from controller
	if (IsValid(controller))
	{
		controller->GetPlayerViewPoint(start_point, start_rot);
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("%s - can't use PlayerController for Interaction, falling back to Owner loc/rot"), *GetName());

	//Last resort
	if (!IsValid(GetOwner()))
	{
		return;
	}
	start_point = GetOwner()->GetActorLocation();
	start_rot = GetOwner()->GetActorRotation();

}

bool UInteractionManager::Interact_Implementation(EInteractionActions action)
{
	if (!interaction_actor.IsValid())
	{
		return false;
	}

	return IInteractionInterface::Execute_OnInteract(interaction_actor.Get(), GetOwner(), action);
}

// Called every frame
void UInteractionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(false);
}

void UInteractionManager::SetInteractionActor(AActor* new_actor)
{
	if (interaction_actor == new_actor)
	{
		return;
	}

	OnInteractionActorChanged.Broadcast(new_actor);
	interaction_actor = new_actor;
}

//void UInteractionManager::SweepInteractionFromView()
//{
//	//DO NOT trace from FirstPersonCamera - it changes rotation unexpectedly (~90deg) (for example when weapon is equipped)
//	//Even though actual view is not affected and mesh head moves very slightly
//
//	FVector sweep_start;
//	FRotator view_point_rot;
//	controller->GetPlayerViewPoint(sweep_start, view_point_rot);
//
//	const auto shape_rot = view_point_rot.Quaternion().GetUpVector().Rotation().Quaternion();
//	const FVector end = sweep_start + view_point_rot.Quaternion().GetForwardVector() * InteractionRange;
//
//	FHitResult hit_result;
//
//	GetWorld()->SweepSingleByChannel(hit_result, sweep_start, end, shape_rot, CollisionChannel, FCollisionShape::MakeSphere(SweepShapeRadius), CollisionParams);
//
//	DrawDebugLine(GetWorld(), sweep_start, end, FColor::Cyan, false, 2, 0, 1.5f);
//
//	OnSweepCompletedSync(hit_result);
//
//}

void UInteractionManager::SweepInteractionFromViewAsync()
{
	//DO NOT trace from FirstPersonCamera - it changes rotation unexpectedly (~90deg) (for example when weapon is equipped)
	//Even though actual view is not affected and mesh head moves very slightly

	FVector sweep_start;
	FRotator view_point_rot;

	GetInteractionStartPoint(sweep_start, view_point_rot);

	const auto shape_rot = view_point_rot.Quaternion().GetUpVector().Rotation().Quaternion();
	const FVector end = sweep_start + view_point_rot.Quaternion().GetForwardVector() * InteractionRange;

	//DrawDebugLine(GetWorld(), sweep_start, end, FColor::Cyan, false, 2, 0, 1.5f);

	GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, sweep_start, end, shape_rot, CollisionChannel, FCollisionShape::MakeSphere(SweepShapeRadius), CollisionParams, {}, &trace_delegate);
}

//void UInteractionManager::OnSweepCompletedSync(const FHitResult& hit_result)
//{
//	AActor* hit_actor = hit_result.GetActor();
//
//	if(hit_actor == nullptr)
//	{
//		//Nothing was hit
//		interaction_actor = nullptr;
//		return;
//	}
//
//	if (hit_actor == interaction_actor)
//	{
//		return;
//	}
//
//	interaction_actor = hit_actor;
//
//	if (interaction_actor->Implements<UInteractionInterface>())
//	{
//		IInteractionInterface* interaction_interface = Cast<IInteractionInterface>(interaction_actor);
//
//		if (interaction_interface->Execute_IsInteractible(interaction_actor))
//		{
//			interaction_interface->Execute_DrawInteractionOutline(interaction_actor);
//			//OpenInteractionUI();
//		}
//	}
//}

void UInteractionManager::OnSweepCompletedAsync(const FTraceHandle& Handle, FTraceDatum& Data)
{
	//Only first hit is considered
	AActor* hit_actor = nullptr;
	if (Data.OutHits.Num() > 0)
	{
		hit_actor = Data.OutHits[0].GetActor();
		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *hit_actor->GetName());

	}

	if (hit_actor == nullptr || hit_actor->Implements<UInteractionInterface>() == false)
	{
		SetInteractionActor(nullptr);
		return;
	}

	if (IInteractionInterface::Execute_IsInteractible(hit_actor))
	{
		SetInteractionActor(hit_actor);

		IInteractionInterface::Execute_DrawInteractionOutline(hit_actor);
	}
}
