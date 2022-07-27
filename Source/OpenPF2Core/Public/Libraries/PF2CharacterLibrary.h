// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>

#include "PF2CharacterLibrary.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;
class UWorld;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Function library for standard PF2 characters.
 */
UCLASS()
class UPF2CharacterLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Gets controllers for all the characters in the world.
	 *
	 * @param World
	 *	A reference to the loaded, running world that contains the characters.
	 *
	 * @return
	 *	PF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Characters")
	static TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetPlayerControllers(const UWorld* const World);

	/**
	 * Gets all of the player-controlled PF2 characters in the world.
	 *
	 * @param World
	 *	A reference to the loaded, running world that contains the characters.
	 *
	 * @return
	 *	PF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintPure, Category="OpenPF2|Characters")
	static TArray<TScriptInterface<IPF2CharacterInterface>> GetPlayerControlledCharacters(const UWorld* const World);
};
