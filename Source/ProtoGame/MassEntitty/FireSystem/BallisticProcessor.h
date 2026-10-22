#pragma once

#include "CoreMinimal.h"
#include "FireSystemFragments.h"

#include "MassProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"

#include "BallisticProcessor.generated.h" 

UCLASS(BlueprintType, ClassGroup = MassEntity)
class PROTOGAME_API UBallisticProcessor : public UMassProcessor
{
    GENERATED_BODY()
public:
    UBallisticProcessor();

protected:
    //virtual void InitializeInternal(UObject& obj, const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
    FVector Gravity = {0, 0, -1};

private:
    FMassEntityQuery BallisticQuery;
};

