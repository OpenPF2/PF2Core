// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayAbilitySpec.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CommandBindingsInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
class UInputComponent;
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CommandBindingsInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for components that maintain Gameplay Ability input action bindings for characters.
 */
class OPENPF2CORE_API IPF2CommandBindingsInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Clears all bindings.
	 *
	 * If input is currently wired up, bindings are removed from input before being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Command Bindings")
	virtual void ClearBindings() = 0;

	/**
	 * Populates the bindings array from the abilities that have been granted to the specified character.
	 *
	 * To prevent duplicate bindings, this can only be called when no bindings have yet been defined or all have been
	 * cleared.
	 *
	 * If input is currently wired up, the new bindings are automatically added to input.
	 *
	 * @param Character
	 *	The character from which to load granted abilities.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Command Bindings")
	virtual void LoadAbilitiesFromCharacter(const TScriptInterface<IPF2CharacterInterface> Character) = 0;

	/**
	 * Populates the bindings array from the abilities that have been granted to the specified character.
	 *
	 * To prevent duplicate bindings, this can only be called when no bindings have yet been defined or all have been
	 * cleared.
	 *
	 * If input is currently wired up, the new bindings are automatically added to input.
	 *
	 * @param Character
	 *	The character from which to load granted abilities.
	 */
	virtual void LoadAbilitiesFromCharacter(IPF2CharacterInterface* Character) = 0;

	/**
	 * Wires-up all bindings to receive input from the given player input component.
	 *
	 * Only new bindings that have an action assigned and have not yet been wired up will be affected; any binding that
	 * was previously wired up will be ignored.
	 *
	 * @param InputComponent
	 *	The component to which input should be bound.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Command Bindings")
	virtual void ConnectToInput(UInputComponent* InputComponent) = 0;

	/**
	 * Removes the association between actions and the input component, allowing all bindings to be re-assigned.
	 *
	 * This must be called before changing the action of any binding.
	 *
	 * Only bindings that have been wired up will be affected; any new binding that is not yet wired up will be ignored.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Command Bindings")
	virtual void DisconnectFromInput() = 0;

	/*
	 * Executes the specified ability on the specified character.
	 *
	 * This is expected to be invoked only by a command binding. This method exists here rather than in the binding
	 * struct itself because RPCs can only be invoked for replicated components, and the command bindings component is
	 * replicated but bindings are not (they are lightweight structs).
	 *
	 * @param AbilitySpecHandle
	 *	The handle for the ability to activate.
	 * @param Character
	 *	The character upon which the ability should be activated.
	 */
	UFUNCTION(Server, Reliable)
	virtual void ExecuteBoundAbility(const FGameplayAbilitySpecHandle AbilitySpecHandle, AActor* Character) = 0;
};
