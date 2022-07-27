// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Libraries/PF2CharacterLibrary.h"

#include <Engine/World.h>

#include "PF2PlayerControllerInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"

TArray<TScriptInterface<IPF2PlayerControllerInterface>> UPF2CharacterLibrary::GetPlayerControllers(
	const UWorld* const World)
{
	TArray<TScriptInterface<IPF2PlayerControllerInterface>> PlayerControllers;

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		IPF2PlayerControllerInterface* const Pf2PlayerController =
			Cast<IPF2PlayerControllerInterface>(PlayerController);

		if (Pf2PlayerController != nullptr)
		{
			PlayerControllers.Add(PF2InterfaceUtilities::ToScriptInterface(Pf2PlayerController));
		}
	}

	return PlayerControllers;
}

TArray<TScriptInterface<IPF2CharacterInterface>> UPF2CharacterLibrary::GetPlayerControlledCharacters(
	const UWorld* const World)
{
	// ReSharper disable once CppRedundantQualifier
	return PF2ArrayUtilities::Reduce<TArray<TScriptInterface<IPF2CharacterInterface>>>(
		UPF2CharacterLibrary::GetPlayerControllers(World),
		TArray<TScriptInterface<IPF2CharacterInterface>>(),
		[](TArray<TScriptInterface<IPF2CharacterInterface>>      Characters,
		   const TScriptInterface<IPF2PlayerControllerInterface> PlayerController)
		{
			Characters.Append(PlayerController->GetControlledCharacters());

			return Characters;
		}
	);
}
