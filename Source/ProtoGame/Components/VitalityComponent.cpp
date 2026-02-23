// Nikita Belov, All rights reserved

#include "VitalityComponent.h"
#include "Character/GameCharacterBase.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UVitalityComponent::UVitalityComponent()
{
	SetupDefaults();

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; //10hz

	NoEnergyEvent.BindDynamic(this, &UVitalityComponent::OnNoEnergy);
	NoHydrationEvent.BindDynamic(this, &UVitalityComponent::OnNoHydration);
	NoHealthEvent.BindDynamic(this, &UVitalityComponent::OnNoHealth);
	NoStaminaEvent.BindDynamic(this, &UVitalityComponent::OnNoStamina);
	NoArmsStaminaEvent.BindDynamic(this, &UVitalityComponent::OnNoArmsStamina);

	//GetOnNoHealth().AddUObject(this, &UVitalityComponent::OnNoHealth);
	//GetOnNoStamina().AddUObject(this, &UVitalityComponent::OnNoStamina);
	//GetOnNoEnergy().AddUObject(this, &UVitalityComponent::OnNoEnergy);
	//GetOnNoHydration().AddUObject(this, &UVitalityComponent::OnNoHydration);
}

void UVitalityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UVitalityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	EnergyDecreaseOverTime(DeltaTime);
	HydrationDecreaseOverTime(DeltaTime);
	HealthRegeneration(DeltaTime);
	TakeHungerDamage(DeltaTime);
	TakeThirstDamage(DeltaTime);
	CalculateStamina(DeltaTime);
	
}

void UVitalityComponent::SetupDefaults()
{
	HealthMax = 100;
	Health = HealthMax;
	bRegeneration = false;
	RegenerationSpeed = 0.1f;
	RegenHealthHighLevel = 80;
	RegenHealthLowLevel = 30;
	bDead = false;
	bAutoRegeneration = false;
	bAllowAutoRegeneration = false;
	HungerDamage = 0.5f;
	ThirstDamage = 0.5f;
	EnergyLevelAllowHealthRegeneration = 25;
	HydrationLevelAllowHealthRegeneration = 25;

	StaminaMax = 100;
	Stamina = StaminaMax;
	StaminaDrainSpeed = 8.0f;
	StaminaRegenSpeed = 6.8f;
	bUsingStamina = false;

	ArmsStaminaMax = 100;
	ArmsStamina = ArmsStaminaMax;
	ArmsStaminaDrainSpeed = 6.2f;
	ArmsStaminaRegenSpeed = 7.0f;
	bUsingArmsStamina = false;

	EnergyMax = 100;
	Energy = EnergyMax;
	EnergyDrainSpeed = 0.01;
	bEnergyRanOut = false;

	HydrationMax = 100;
	Hydration = HydrationMax;
	HydrationDrainSpeed = 0.012;
	bHydrationRanOut = false;
}


void UVitalityComponent::ChangeHealth(float value)
{
	if((value + Health) > HealthMax)
	{
		Health = HealthMax;
	}
	else
	{
		Health += value;
	}

	if(Health <= 0.f)
	{
		Health = 0;
		NoHealthEvent.Execute();
	}

}

void UVitalityComponent::HealthRegeneration(float DeltaTime)
{
	if(bAllowAutoRegeneration)
	{
		if(Health < RegenHealthHighLevel && RegenHealthLowLevel <= Health &&
			Energy >= EnergyLevelAllowHealthRegeneration && Hydration >= HydrationLevelAllowHealthRegeneration)
		{
			bAutoRegeneration = true;
			ChangeHealth(DeltaTime * RegenerationSpeed);
		}
		else
		{
			bAutoRegeneration = false;
		}
	}
}

void UVitalityComponent::TakeHungerDamage(float DeltaTime)
{
	if(Energy <= 0)
	ChangeHealth(-1 * HungerDamage * DeltaTime);
}

void UVitalityComponent::TakeThirstDamage(float DeltaTime)
{
	if(Hydration <= 0)
	ChangeHealth(-1 * ThirstDamage * DeltaTime);
}

void UVitalityComponent::OnNoHealth()
{
	bDead = true;
	SetComponentTickEnabled(false);

	Cast<AGameCharacterBase>(GetOwner())->SetDeathState(true);

}

void UVitalityComponent::CalculateStamina(float DeltaTime)
{
	AGameCharacterBase* CharacterOwner = Cast<AGameCharacterBase>(GetOwner());

	if(bUsingStamina == true && 
		(CharacterOwner->GetCharacterMovement()->IsMovingOnGround() && 
		(CharacterOwner->GetInputAxisValue("MoveRight") != 0 || CharacterOwner->GetInputAxisValue("MoveForward") != 0)))
	{
		if(Stamina > 0)
		{
			Stamina -= StaminaDrainSpeed * DeltaTime;
		}
		else
		{
			Stamina = 0;
			//NoStaminaEvent.Execute();
		}
	}
	else //regen
	{
		if(Stamina < StaminaMax)
		{
			Stamina += StaminaDrainSpeed * DeltaTime;
		}
		else
		{
			Stamina = StaminaMax;
		}
	}
}

void UVitalityComponent::OnNoStamina()
{

}

void UVitalityComponent::CalculateArmsStamina(float DeltaTime)
{
	if(bUsingArmsStamina == true) 
	{
		if(ArmsStamina > 0)
		{
			ArmsStamina -= ArmsStaminaDrainSpeed * DeltaTime;
		}
		else
		{
			ArmsStamina = 0;
			NoArmsStaminaEvent.Execute();
		}
	}
	else //regen
	{
		if(ArmsStamina < ArmsStaminaMax)
		{
			ArmsStamina += ArmsStaminaDrainSpeed * DeltaTime;
		}
		else
		{
			ArmsStamina = ArmsStaminaMax;
		}
	}
}

void UVitalityComponent::OnNoArmsStamina()
{

}

void UVitalityComponent::StartUsingStamina()
{
	bUsingStamina = true;
}

void UVitalityComponent::StopUsingStamina()
{
	bUsingStamina = false;
}

void UVitalityComponent::StartUsingArmsStamina()
{
	bUsingArmsStamina = true;
}

void UVitalityComponent::StopUsingArmsStamina()
{
	bUsingArmsStamina = false;
}

void UVitalityComponent::ChangeEnergy(float value)
{
	if((value + Energy) > EnergyMax)
	{
		Energy = EnergyMax;
	}
	else
	{
		Energy += value;
	}

	if(Energy <= 0.f)
	{
		Energy = 0;
		bEnergyRanOut = true;
		//NoEnergyEvent.Execute();
	}
	else
	{
		bEnergyRanOut = false;
	}
	
}

void UVitalityComponent::EnergyDecreaseOverTime(float DeltaTime)
{
	ChangeEnergy(-1 * EnergyDrainSpeed * DeltaTime);
}

void UVitalityComponent::OnNoEnergy()
{
	//TakeHungerDamage()
}

void UVitalityComponent::ChangeHydration(float value)
{
	if((value + Hydration) > HydrationMax)
	{
		Hydration = HydrationMax;
	}
	else
	{
		Hydration += value;
	}

	if(Hydration <= 0.f)
	{
		Hydration = 0;
		bHydrationRanOut = true;
		//NoHydrationEvent.Execute();
	}
	else
	{
		bHydrationRanOut = false;
	}
}

void UVitalityComponent::HydrationDecreaseOverTime(float DeltaTime)
{
	ChangeHydration(-1 * HydrationDrainSpeed * DeltaTime);
}

void UVitalityComponent::OnNoHydration()
{
	//TakeThirstDamage()
}
