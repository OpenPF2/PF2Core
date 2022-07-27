// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "PF2GameplayAbilityInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
class UTexture2D;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UPF2GameplayAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Gameplay Abilities.
 */
class OPENPF2CORE_API IPF2GameplayAbilityInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets an icon to represent this ability, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A graphical representation of this ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual UTexture2D* GetAbilityIcon() const = 0;

	/**
	 * Gets the name of this ability, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A short textual description of this ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual FText GetAbilityLabel() const = 0;

	/**
	 * Gets the description of this ability, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A long textual description of this ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual FText GetAbilityDescription() const = 0;

	/**
	 * Gets the default human-friendly input action that triggers this ability (if applicable).
	 * If left blank, this ability has no default binding and must be assigned a binding at run-time.
	 *
	 * @return
	 *	The name of the input action that should trigger this ability by default.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Gameplay Abilities")
	virtual FName GetDefaultInputActionMapping() const = 0;
};
