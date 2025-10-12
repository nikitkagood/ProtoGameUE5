// Nikita Belov, All rights reserved

#include "MoverNavPawn.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "DefaultMovementSet/NavMoverComponent.h"
#include "GameFramework/PlayerController.h"
#include "DefaultMovementSet/Settings/CommonLegacyMovementSettings.h"

DEFINE_LOG_CATEGORY(LogMoverPawn)

struct FMoverInputCmdContext;
struct FCharacterDefaultInputs;

// Sets default values
AMoverNavPawn::AMoverNavPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMoverComponent = CreateDefaultSubobject<UCharacterMoverComponent>("Character Mover Comp");
	ensure(CharacterMoverComponent);

	NavMoverComponent = CreateDefaultSubobject<UNavMoverComponent>("Nav Mover Comp");

	PrimaryActorTick.bCanEverTick = true;

	SetReplicatingMovement(false);	// disable Actor-level movement replication, since our Mover component will handle it

	//Navigation system uses 44 deg, default value is 0.71 which is smaller than exactly 44 deg
	//thus Navigation may sent Pawn into a place where it will get stuck
	//That's why we set 0.7195 - tiny bit more than 44 deg 
	//Since Mover variable access is complicated, it's easier to set in BP
	//auto* settings_ptr = CharacterMoverComponent->FindSharedSettings_Mutable<UCommonLegacyMovementSettings>();
	//if (settings_ptr) { settings_ptr->MaxWalkSlopeCosine = 0.7195; }
}

// Called when the game starts or when spawned
void AMoverNavPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->PlayerCameraManager->ViewPitchMax = 89.0f;
		PC->PlayerCameraManager->ViewPitchMin = -89.0f;
	}
}

// Called every frame
void AMoverNavPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMoverNavPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMoverNavPawn::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd)
{
	//Example code that allows BP override. We skip it.
	
	//OnProduceInput((float)SimTimeMs, InputCmdResult);

	//if (bHasProduceInputinBpFunc)
	//{
	//	InputCmdResult = OnProduceInputInBlueprint((float)SimTimeMs, InputCmdResult);
	//}

	// Generate user commands. Called right before the Character movement simulation will tick (for a locally controlled pawn)
	// This isn't meant to be the best way of doing a camera system. It is just meant to show a couple of ways it may be done
	// and to make sure we can keep distinct the movement, rotation, and view angles.
	// Styles 1-3 are really meant to be used with a gamepad.
	//
	// Its worth calling out: the code that happens here is happening *outside* of the Character movement simulation. All we are doing
	// is generating the input being fed into that simulation. That said, this means that A) the code below does not run on the server
	// (and non controlling clients) and B) the code is not rerun during reconcile/resimulates. Use this information guide any
	// decisions about where something should go (such as aim assist, lock on targeting systems, etc): it is hard to give absolute
	// answers and will depend on the game and its specific needs. In general, at this time, I'd recommend aim assist and lock on 
	// targeting systems to happen /outside/ of the system, i.e, here. But I can think of scenarios where that may not be ideal too.

	//CharacterInputs is what has to be set in order for things to work
	FCharacterDefaultInputs& CharacterInputs = OutInputCmd.InputCollection.FindOrAddMutableDataByType<FCharacterDefaultInputs>();

	//CharacterMoverComponent->GetLastInputCmd().InputCollection.FindMutableDataByType<FCharacterDefaultInputs>();

	if (Controller == nullptr)
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
		{
			static const FCharacterDefaultInputs DoNothingInput;
			// If we get here, that means this pawn is not currently possessed and we're choosing to provide default do-nothing input
			CharacterInputs = DoNothingInput;
		}

		// We don't have a local controller so we can't run the code below. This is ok. Simulated proxies will just use previous input when extrapolating
		return;
	}

	//TODO: Unreal 5.6 changed variables, needs fixing
	bool bRequestedNavMovement = false;
	if (NavMoverComponent)
	{
		bRequestedNavMovement = NavMoverComponent->ConsumeNavMovementData(ControlInputVector, CachedMoveInputVelocity);
	}

	//Example suggest favoring Velocity input
	//But we keyboard wise we favor Input intent
	bool bUsingInputIntentForMove = CachedMoveInputVelocity.IsZero();

	if (bUsingInputIntentForMove)
	{
		const FVector FinalDirectionalIntent = CharacterInputs.ControlRotation.RotateVector(ControlInputVector);
		CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, FinalDirectionalIntent);
	}
	else
	{
		CharacterInputs.SetMoveInput(EMoveInputType::Velocity, CachedMoveInputVelocity);
	}

	//AMoverExamplesCharacter does it this way: BindAction(MoveInputAction, ETriggerEvent::Completed, this, &AMoverExamplesCharacter::OnMoveCompleted)
	//But we could just do this, especially since Nav movement isn't BindAction and nothing is called like this
	ControlInputVector = FVector::ZeroVector;
	CachedMoveInputVelocity = FVector::ZeroVector;



	static float RotationMagMin(1e-3);

	const bool bHasAffirmativeMoveInput = (CharacterInputs.GetMoveInput().Size() >= RotationMagMin);

	// Figure out intended orientation

	CharacterInputs.OrientationIntent = FVector::ZeroVector;
	CharacterInputs.ControlRotation = Controller->GetControlRotation();

	if (bHasAffirmativeMoveInput)
	{
		if (bOrientRotationToMovement)
		{
			// set the intent to the actors movement direction
			CharacterInputs.OrientationIntent = CharacterInputs.GetMoveInput().GetSafeNormal();
			//CharacterInputs.OrientationIntent = FMath::VInterpConstantTo(CharacterInputs.OrientationIntent, CharacterInputs.GetMoveInput().GetSafeNormal(), GetWorld()->DeltaTimeSeconds, MovementInterpolationSpeed);
		}
		else
		{
			// set intent to the the control rotation - often a player's camera rotation
			CharacterInputs.OrientationIntent = CharacterInputs.ControlRotation.Vector().GetSafeNormal();
		}

		LastControlInputVector = CharacterInputs.GetMoveInput();
	}
	else if (bMaintainLastInputOrientation)
	{
		// There is no movement intent, so use the last-known affirmative move input
		CharacterInputs.OrientationIntent = LastControlInputVector;
	}

	if (bShouldRemainVertical)
	{
		// canceling out any z intent if the actor is supposed to remain vertical
		CharacterInputs.OrientationIntent = CharacterInputs.OrientationIntent.GetSafeNormal2D();
	}


	CharacterInputs.bIsJumpPressed = bIsJumpPressed;
	CharacterInputs.bIsJumpJustPressed = bIsJumpJustPressed;

	if (bIsFlyingAllowed)
	{
		if (bIsFlying)
		{
			CharacterInputs.SuggestedMovementMode = DefaultModeNames::Flying;
		}
		else
		{
			CharacterInputs.SuggestedMovementMode = DefaultModeNames::Falling;
		}
	}
	else
	{
		//MoverExamplesCharacter but we might not need it
		//CharacterInputs.SuggestedMovementMode = NAME_None;
	}



	// Convert inputs to be relative to the current movement base (depending on options and state)
	CharacterInputs.bUsingMovementBase = false;

	if (bUseBaseRelativeMovement)
	{
		if (const UCharacterMoverComponent* MoverComp = GetComponentByClass<UCharacterMoverComponent>())
		{
			if (UPrimitiveComponent* MovementBase = MoverComp->GetMovementBase())
			{
				FName MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

				FVector RelativeMoveInput, RelativeOrientDir;

				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.GetMoveInput(), RelativeMoveInput);
				UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.OrientationIntent, RelativeOrientDir);

				CharacterInputs.SetMoveInput(CharacterInputs.GetMoveInputType(), RelativeMoveInput);
				CharacterInputs.OrientationIntent = RelativeOrientDir;

				CharacterInputs.bUsingMovementBase = true;
				CharacterInputs.MovementBase = MovementBase;
				CharacterInputs.MovementBaseBoneName = MovementBaseBoneName;
			}
		}
	}

	// Clear/consume temporal movement inputs. We are not consuming others in the event that the game world is ticking at a lower rate than the Mover simulation. 
	// In that case, we want most input to carry over between simulation frames.
	{

		//bIsJumpJustPressed = false;
		//bShouldToggleFlying = false;
	}

}

FVector AMoverNavPawn::GetNavAgentLocation() const
{
	FVector AgentLocation = FNavigationSystem::InvalidLocation;

	const USceneComponent* UpdatedComponent = nullptr;

	if (CharacterMoverComponent)
	{
		UpdatedComponent = CharacterMoverComponent->GetUpdatedComponent();
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("AMoverNavPawn::GetNavAgentLocation: CharacterMoverComponent is invalid"));
	}

	if (NavMoverComponent)
	{
		AgentLocation = NavMoverComponent->GetFeetLocation();
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("AMoverNavPawn::GetNavAgentLocation: NavMoverComponent is invalid"));
	}

	if (FNavigationSystem::IsValidLocation(AgentLocation) == false && UpdatedComponent != nullptr)
	{
		AgentLocation = UpdatedComponent->GetComponentLocation() - FVector(0, 0, UpdatedComponent->Bounds.BoxExtent.Z);
	}

	return AgentLocation;
}

void AMoverNavPawn::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	//MovementComponent->AddInputVector(WorldDirection * ScaleValue, bForce);
	
	//CharacterMoverComponent

	//if (PawnOwner)
	{

		if (bForce || !IsMoveInputIgnored())
		{
			ControlInputVector += WorldDirection * ScaleValue;
		}

	}
}

const FMoverDefaultSyncState* AMoverNavPawn::GetMoverDefaultSyncState() const
{
	return CharacterMoverComponent ? CharacterMoverComponent->GetSyncState().SyncStateCollection.FindDataByType<FMoverDefaultSyncState>() : nullptr;
}

FMoverDefaultSyncState* AMoverNavPawn::GetMoverDefaultSyncStateMutable() const
{
	return CharacterMoverComponent ? CharacterMoverComponent->GetSyncState().SyncStateCollection.FindMutableDataByType<FMoverDefaultSyncState>() : nullptr;
}


void AMoverNavPawn::OnJumpStarted(const FInputActionValue& Value)
{
	bIsJumpJustPressed = !bIsJumpPressed;
	bIsJumpPressed = true;
}

void AMoverNavPawn::OnJumpReleased(const FInputActionValue& Value)
{
	bIsJumpPressed = false;
	bIsJumpJustPressed = false;
}

void AMoverNavPawn::OnFlyTriggered(const FInputActionValue& Value)
{
	bIsFlying = !bIsFlying;
}