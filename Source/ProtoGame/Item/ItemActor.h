// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interfaces/InteractionInterface.h"
#include "Item/ItemInfo.h"
#include "Item/ItemThumbnailInfo.h"

#include "ItemActor.generated.h"

class UItemBase;
class UStaticMeshComponent;
class UDataTable;

UENUM(BlueprintType)
enum class ItemObjectCreationMethod : uint8
{
	//Create an item object from DataTable row  
	CreateItemObjectFromDataTable      UMETA(DisplayName = "Create item object from data table"),

	//Allows to add additional functionality via blueprint
	//Not in use currently
	CreateItemObjectFromItemBaseBP     UMETA(DisplayName = "Create item object from ItemBase BP"),
};

UENUM(BlueprintType)
enum class ItemActorMeshType : uint8
{
	StaticMesh       UMETA(DisplayName = "Static Mesh"),
	SkeletalMesh     UMETA(DisplayName = "Skeletal Mesh"),
};


//This class represents items with physical location, located in a world.
//Actual properties of an item are represented by world-independent, UObject-derived class (UItemBase).
//Note that items attached to a character ARE NOT considered ItemActors. They are ItemBase + SkeletalMesh (although this might change).
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, meta=(BlueprintSpawnableComponent), ClassGroup=(Item))
class PROTOGAME_API AItemActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	//Only 1 mesh component is allowed to be active at the time

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true", EditCondition = "item_actor_mesh_type == ItemActorMeshType::SkeletalMesh", EditConditionHides))
	USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true", EditCondition = "item_actor_mesh_type == ItemActorMeshType::StaticMesh", EditConditionHides))
	UStaticMeshComponent* StaticMeshComp;
	
public:	
	AItemActor();

	FDataTableRowHandle GetItemProperites() { return DT_ItemProperties; };

	//Deffered spawn is used to set ItemBase
	//Spawns ItemActor from ItemBase. Sets new ItemActor to be Outer for the ItemBase.
	static AItemActor* StaticCreateObject(UWorld* world, TSubclassOf<AItemActor> item_actor_class, UItemBase* item_object, const FVector& location, const FRotator& rotation);
	//Only visual: doesn't set ItemBase Outer and non-interactable.
	static AItemActor* StaticCreateObjectVisualOnly(UWorld* world, TSubclassOf<AItemActor> item_actor_class, UItemBase* item_object, const FVector& location, const FRotator& rotation);

	virtual void Tick(float DeltaTime) override;

	//IInteractionInterface

	//See OnInteract_Implementation
	//Even though it's already virtual, don't type "virtual bool" - it will cause compiler error
	UFUNCTION(BlueprintNativeEvent)
	bool OnInteract(AActor* caller) override; 

	//UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteractible_Implementation() const { return bInteractable; };

	//UFUNCTION(BlueprintCallable, Category = "Interaction")
	InteractionType GetInteractionType_Implementation() const { return InteractionType::Item; };

	void DrawInteractionOutline_Implementation();

	void StopDrawingOutline_Implementation();

	//UFUNCTION(BlueprintCallable)
	//UStaticMesh* GetMesh() { return StaticMeshComp->GetStaticMesh(); };

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetSkeletalMeshComp() { return SkeletalMeshComp; };

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMeshComp() { return StaticMeshComp; };

	void SetItemObject(UItemBase* item_object) { ItemObject = item_object; };

	void SetAnimClass(TSubclassOf<UAnimInstance> anim_class);

	UFUNCTION(BlueprintCallable)
	void AttachItemActorToComponent(USceneComponent* Parent, EAttachmentRule AttachmentRule, bool weld, FName SocketName);

protected:
	//Deffered spawn is used to set ItemObject
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor", meta = (AllowPrivateAccess = true))
	bool bInteractable;
private:
	void CreateItemObject();

	bool OnInteract_Implementation(AActor* caller);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Actor", meta = (AllowPrivateAccess = "true"))
	ItemActorMeshType item_actor_mesh_type = ItemActorMeshType::StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Object", meta = (AllowPrivateAccess = "true"))
	ItemObjectCreationMethod item_object_creation_method;

	//associated item object
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Instanced, Category = "Item Object", NoClear, meta = (AllowPrivateAccess = "true"))
	UItemBase* ItemObject;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, NoClear, Category = "Item Object", meta = (AllowPrivateAccess = true))
	TSubclassOf<UItemBase> ItemBaseClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, NoClear, Category = "Item Object", meta = (AllowPrivateAccess = "true", 
	EditCondition = "item_object_creation_method == ItemObjectCreationMethod::CreateItemObjectFromDataTable", EditConditionHides))
	FDataTableRowHandle DT_ItemProperties;
};
