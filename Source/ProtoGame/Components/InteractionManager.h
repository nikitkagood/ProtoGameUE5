// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "Engine/EngineTypes.h"
#include "WorldCollision.h"
//#include "CollisionQueryParams.h"

#include "Interfaces/InteractionInterface.h"

#include "InteractionManager.generated.h"

struct FTraceHandle;
struct FTraceDatum;
struct FCollisionQueryParams;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionActorChanged, AActor*, new_actor);

//Player interaction manager
//AController is expected
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Component), meta=(BlueprintSpawnableComponent) )
class PROTOGAME_API UInteractionManager : public UActorComponent
{
	GENERATED_BODY()


public:	
	UInteractionManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetInteractionActor(AActor* new_actor);

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	//void OpenInteractionUI();

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "UI")
	//void CloseInteractionUI();

protected:
	virtual void BeginPlay() override;

	//PlayerController -> GetPlayerViewPoint is default
	//Otherwise Owner -> GetLocation
	//Override in BP if needed
	UFUNCTION(BlueprintNativeEvent)
	void GetInteractionStartPoint(FVector& start_point, FRotator& start_rot);

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	//EInteractionActions GetInteractionAction();

	//Interact with currently active Actor
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Interact(EInteractionActions action);

private:
	//UFUNCTION()
	//void SweepInteractionFromView();

	UFUNCTION()
	void SweepInteractionFromViewAsync();

	//Callback for AsyncSweep
	void OnSweepCompletedAsync(const FTraceHandle& Handle, FTraceDatum& Data);

	//Check if any actor within sphere is interactible
	//Interactible actors must have GenerateOverlapEvents = true
	UFUNCTION()
	void SphereCollisionCheck();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	//UFUNCTION()
	//void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	//	bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionActorChanged OnInteractionActorChanged;

	//Trace (sweep) distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float InteractionRange = 130.f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_Visibility;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FCollisionQueryParams CollisionParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;


	//False: trace (sweep) all the time
	//True: make sure anything interactible is any close before ever starting to trace
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool UseCollisionOptimization = true;

	//Since "trace" is actually sweep. Acts like line thickness.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SweepShapeRadius = 2.25f;

	//At some point, it's easier to just always trace, rather than to iterate over too many actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int ProximityActorIterationLimit = 50;

private:
	FTimerHandle interaction_loop_timer_handle;
	FTraceDelegate trace_delegate;

	FTimerHandle sphere_collision_timer_handle;

	//By default we try get GetPlayerViewPoint from it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AController* controller;

	//Store actor which we gonna interact with
	//Only use setter
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> interaction_actor;
};
