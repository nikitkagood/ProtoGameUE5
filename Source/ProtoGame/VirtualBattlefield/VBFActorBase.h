// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Interfaces/VBFUnitInterface.h"
#include "Pawn/MoverNavPawn.h"

#include "VBFActorBase.generated.h"

class UVBFUnitBase;
class UPawnMovement;

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API AVBFActorBase : public AMoverNavPawn
{
	GENERATED_BODY()
	
public:	
	AVBFActorBase();

	//UGameplayStatics::FinishSpawningActor has to be called manually
	static AVBFActorBase* StaticCreateObjectDeferred(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FTransform& transform, ESpawnActorCollisionHandlingMethod collision_handling = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//virtual bool Initialize(UStreamableRenderAsset* render_asset);

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IVBFUnitInterface> GetVBFUnitInterface(bool& is_valid);

	UVBFUnitBase* GetVBFUnit() { return vbf_unit; };

	const UVBFUnitBase* GetVBFUnit() const { return vbf_unit; };

	UFUNCTION()
	void SetVBFUnit(UVBFUnitBase* unit) { vbf_unit = unit; };

	//UFUNCTION(BlueprintCallable)
	//bool GetVBFUnitInterface(bool& is_valid);

	//~ Begin APawn Interface.
	//virtual void PostInitializeComponents() override;
	//virtual UPawnMovementComponent* GetMovementComponent() const override;
	//virtual UPrimitiveComponent* GetMovementBase() const override final { return BasedMovement.MovementBase; }
	//virtual float GetDefaultHalfHeight() const override;
	//virtual void TurnOff() override;
	//virtual void Restart() override;
	//virtual void PawnClientRestart() override;
	//virtual void PossessedBy(AController* NewController) override;
	//virtual void UnPossessed() override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	//virtual void RecalculateBaseEyeHeight() override;
	//virtual void UpdateNavigationRelevance() override;
	//~ End APawn Interface


protected:
	virtual void BeginPlay() override;

	//UFUNCTION(BlueprintCallable)
	//bool SetupMeshComponent(UStreamableRenderAsset* render_asset);

	//It's either Static or Skeletal
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//TObjectPtr<UMeshComponent> MeshComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//UStreamableRenderAsset* MeshRenderAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UVBFUnitBase> vbf_unit;

};
