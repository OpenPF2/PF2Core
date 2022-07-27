// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "PF2CharacterInterface.h"
#include "PF2GameStateInterface.h"
#include "PF2ModeOfPlayType.h"

#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "PF2ModeOfPlayRuleSetInterface.generated.h"

UINTERFACE(BlueprintType)
class OPENPF2CORE_API UPF2ModeOfPlayRuleSetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Mode of Play Rule Sets (MoPRS), which provide strategies for the game should
 * behave while in a particular mode of PF2 play.
 *
 * MoPRS not only provide logic that control how different gameplay events are handled, but also can act as an extension
 * to game state by storing and maintaining variables that are relevant for the current mode of play. For example,
 * encounter modes maintain initiative order, number of enemies left standing, etc. which are not relevant in other game
 * modes like exploration mode. A new MoPRS instance is created each time that the mode of play changes, so this state
 * is only maintained while it is relevant.
 *
 * @see EPF2ModeOfPlayType
 */
class OPENPF2CORE_API IPF2ModeOfPlayRuleSetInterface
{
	GENERATED_BODY()

public:
	/**
	 * Callback to notify this rule set that the mode of play that invoked it is now active.
	 *
	 * The rule set should use this as an opportunity to initialize its state (e.g., roll initiative, assemble a list of
	 * enemies, etc.).
	 *
	 * @param ModeOfPlay
	 *	The mode of play that is just starting.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void OnModeOfPlayStart(EPF2ModeOfPlayType ModeOfPlay);

	/**
	 * Callback to notify this rule set that a playable character has just joined the game.
	 *
	 * This only fires if a player joins the game while this rule set is already active. The rule set should use this as
	 * an opportunity to update character-tracking logic (e.g., add the player to an existing, on-going encounter,
	 * etc.).
	 *
	 * @param Character
	 *	The character of the player who just connected.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void OnPlayableCharacterStarting(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character should be added to the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void OnCharacterAddedToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character should be removed from the current encounter.
	 *
	 * The rule set can choose to ignore this event if it's not applicable (e.g., this rule set is not for an
	 * encounter). This callback should also be ignored if the given character is not part of the encounter.
	 *
	 * @param Character
	 *	The character being removed from the encounter.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void OnCharacterRemovedFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Callback to notify this rule set that a character wishes to perform a command (e.g., use an ability).
	 *
	 * This gives the rule set control over when the command should be performed (e.g., to enforce initiative order).
	 * The command may not get executed if the encounter ends before it has been activated. In such a situation, the
	 * command will be canceled instead.
	 *
	 * @param Character
	 *	The character that is queuing the command up.
	 * @param Command
	 *	The command that is being queued.
	 *
	 * @return
	 *	A result that indicates whether the command was queued, executed immediately, or refused.
	 */
	UFUNCTION(BlueprintNativeEvent, Category="OpenPF2|Mode of Play Rule Sets")
	EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand(
		const TScriptInterface<IPF2CharacterInterface>&    Character,
	    const TScriptInterface<IPF2CharacterCommandInterface>& Command);

	/**
	 * Determines whether this rule set allows transitioning to the specified mode of play with the given game state.
	 *
	 * Some modes freely allow transitions to other game modes, while others place restrictions on transitions so that
	 * they depend on certain conditions being met. For example, it is common that encounters prevent players from
	 * returning to exploration until they have either: 1) vanquished all enemies within range; 2) retreated
	 * successfully; or 3) forced remaining enemies to retreat. Meanwhile, it is common for players to be able to leave
	 * exploration and downtime modes of play without any conditions.
	 *
	 * @param GameState
	 *	The current game state.
	 * @param TargetMode
	 *	The mode of play to which the game is attempting to transition.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	bool CanTransitionTo(const TScriptInterface<IPF2GameStateInterface>& GameState,
	                     const EPF2ModeOfPlayType                        TargetMode) const;

	/**
	 * Callback to notify this rule set to wrap-up prior to a change in mode of play.
	 *
	 * The rule set should use this as an opportunity to apply any long-lasting effects of the mode (e.g., calculate
	 * experience and hero points, end encounter-only gameplay effects or abilities, etc.).
	 *
	 * @param ModeOfPlay
	 *	The mode of play that is ending.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Mode of Play Rule Sets")
	void OnModeOfPlayEnd(EPF2ModeOfPlayType ModeOfPlay);
};
