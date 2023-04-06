// Nikita Belov, All rights reserved

#pragma once

#include "CoreMinimal.h"

#include "VitalityComponent.generated.h"

class UActorComponent;
class ACharacter;
class AGameCharacterBase;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_DELEGATE(FNoEnergyEvent);
DECLARE_DYNAMIC_DELEGATE(FNoHydrationEvent);
DECLARE_DYNAMIC_DELEGATE(FNoHealthEvent);
DECLARE_DYNAMIC_DELEGATE(FNoStaminaEvent);
DECLARE_DYNAMIC_DELEGATE(FNoArmsStaminaEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOGAME_API UVitalityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVitalityComponent();

protected:
	virtual void BeginPlay() override;

public:
	//Health
	UFUNCTION(BlueprintCallable)
	void ChangeHealth(float value);

protected:
	UFUNCTION()
	void HealthRegeneration(float DeltaTime);

	UFUNCTION()
	void TakeHungerDamage(float DeltaTime);

	UFUNCTION()
	void TakeThirstDamage(float DeltaTime);

	UFUNCTION()
	void OnNoHealth();

	//Stamina
	UFUNCTION()
	void CalculateStamina(float DeltaTime);

	UFUNCTION()
	void OnNoStamina();

	UFUNCTION()
	void CalculateArmsStamina(float DeltaTime);

	UFUNCTION()
	void OnNoArmsStamina();

public:
	UFUNCTION()
	void StartUsingStamina();

	UFUNCTION()
	void StopUsingStamina();

	UFUNCTION()
	void StartUsingArmsStamina();

	UFUNCTION()
	void StopUsingArmsStamina();

protected:
	//Energy
    UFUNCTION()
	void ChangeEnergy(float value);

	UFUNCTION()
	void EnergyDecreaseOverTime(float DeltaTime);

	UFUNCTION()
	void OnNoEnergy();

	//Hydration
	UFUNCTION()
	void ChangeHydration(float value);

	UFUNCTION()
	void HydrationDecreaseOverTime(float DeltaTime);

	UFUNCTION()
	void OnNoHydration();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void SetupDefaults();

	UPROPERTY()
	FNoEnergyEvent NoEnergyEvent;

	UPROPERTY()
	FNoHydrationEvent NoHydrationEvent;

	UPROPERTY()
	FNoHealthEvent NoHealthEvent;

	UPROPERTY()
	FNoStaminaEvent NoStaminaEvent;

	UPROPERTY()
	FNoArmsStaminaEvent NoArmsStaminaEvent;

	//Health
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	float HealthMax;

	UPROPERTY(VisibleAnywhere, Category = Health)
	bool bRegeneration;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float RegenerationSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float RegenHealthHighLevel;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float RegenHealthLowLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	bool bDead;

	UPROPERTY(VisibleAnywhere, Category = Health)
	bool bAutoRegeneration;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	bool bAllowAutoRegeneration;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	float HungerDamage;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	float ThirstDamage;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Health)
	float EnergyLevelAllowHealthRegeneration;

	UPROPERTY(VisibleAnywhere, Category = Health)
	float HydrationLevelAllowHealthRegeneration;

	//Stamina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float Stamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float StaminaMax;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float StaminaDrainSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float StaminaRegenSpeed;

	UPROPERTY(VisibleAnywhere, Category = Stamina)
	bool bUsingStamina;

	//Arms stamina
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float ArmsStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Stamina)
	float ArmsStaminaMax;

	UPROPERTY(EditDefaultsOnly, Category = Stamina)
	float ArmsStaminaDrainSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Stamina)
	float ArmsStaminaRegenSpeed;

	UPROPERTY(VisibleAnywhere, Category = Stamina)
	bool bUsingArmsStamina;

	//Energy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Energy)
	float Energy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Energy)
	float EnergyMax;

	UPROPERTY(EditDefaultsOnly, Category = Energy)
	float EnergyDrainSpeed;

	UPROPERTY(VisibleAnywhere, Category = Energy)
	bool bEnergyRanOut;

	//Hydration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Hydration)
	float Hydration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Hydration)
	float HydrationMax;

	UPROPERTY(EditDefaultsOnly, Category = Hydration)
	float HydrationDrainSpeed;

	UPROPERTY(VisibleDefaultsOnly, Category = Hydration)
	bool bHydrationRanOut;
};
