#include "InteractionInterface.h"
// Nikita Belov, All rights reserved

//inline void IInteractionInterface::InteractCombined(AActor* iteracted_actor, AActor* caller)
//{
//	//Check if this object implements the interface. Works for both C++ and BP.
//	if (iteracted_actor->Implements<UInteractionInterface>())
//	{
//		IInteractionInterface* interaction_interface = Cast<IInteractionInterface>(iteracted_actor);
//
//		//We checked if the interface is implemented already. But Cast won't work for BP objects.
//		//Thus null -> call BP function, even though nullptr calls look kind of scary
//		if (interaction_interface != nullptr)
//		{
//			interaction_interface->OnInteract(caller);
//		}
//		else
//		{
//			interaction_interface->Execute_OnInteractBP(iteracted_actor, caller);
//		}
//	}
//}
