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
	
	friend UVBFUnitBase;
public:	
	AVBFActorBase();

	virtual void Tick(float DeltaTime) override;


	UVBFUnitBase* GetVBFUnit() { return vbf_unit; };

	const UVBFUnitBase* GetVBFUnit() const { return vbf_unit; };

	void SetVBFUnit(UVBFUnitBase* unit) { vbf_unit = unit; };
protected:
	virtual void BeginPlay() override;

	static AVBFActorBase* StaticCreateObject(UWorld* world, TSubclassOf<AActor> actor_class, UVBFUnitBase* unit_object, const FVector& location, const FRotator& rotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UVBFUnitBase* vbf_unit;

};
