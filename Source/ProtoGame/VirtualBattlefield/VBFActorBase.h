// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/VBFUnitInterface.h"

#include "VBFActorBase.generated.h"

class UVBFUnitBase;

UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class PROTOGAME_API AVBFActorBase : public AActor //, public IVBFUnitInterface
{
	GENERATED_BODY()
	
public:	
	AVBFActorBase();

	static AVBFActorBase* StaticCreateObject(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FTransform& transform);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual bool Initialize(UStreamableRenderAsset* render_asset);

	UVBFUnitBase* GetVBFUnit() { return vbf_unit; };

	const UVBFUnitBase* GetVBFUnit() const { return vbf_unit; };

	UFUNCTION()
	void SetVBFUnit(UVBFUnitBase* unit) { vbf_unit = unit; };

	//UFUNCTION(BlueprintCallable)
	//UStreamableRenderAsset* GetMeshRenderAsset() { return MeshRenderAsset; };

	//UFUNCTION(BlueprintCallable)
	//void SetMeshRenderAsset(UStreamableRenderAsset* render_asset);
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool SetupMeshComponent(UStreamableRenderAsset* render_asset);

	//It's either Static or Skeletal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UMeshComponent* MeshComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	//UStreamableRenderAsset* MeshRenderAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UVBFUnitBase* vbf_unit;

};
