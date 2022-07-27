// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>
#include <UObject/ScriptInterface.h>

#include "PF2ModeOfPlayRuleSetInterface.h"

#include "Commands/PF2CharacterCommandInterface.h"
#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "PF2GameModeInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2GameModeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Player Modes.
 *
 * Instances of this interface exist only on the SERVER, as is the case with a game mode in any UE-powered game.
 */
class OPENPF2CORE_API IPF2GameModeInterface
{
	GENERATED_BODY()

public:
	/**
	 * Creates the appropriate rule set for the given mode of play.
	 *
	 * @return
	 *	The rule set for the current mode of play.
	 */
	virtual TScriptInterface<IPF2ModeOfPlayRuleSetInterface> CreateModeOfPlayRuleSet(const EPF2ModeOfPlayType ModeOfPlay) = 0;

	/**
	 * Requests a switch of the play mode to encounter mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual void RequestEncounterMode() = 0;

	/**
	 * Requests a switch of the play mode to exploration mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual void RequestExplorationMode() = 0;

	/**
	 * Requests a switch of the play mode to downtime mode.
	 *
	 * The request is subject to any business rules of the game mode. If the switch is not allowed by current game
	 * rules, the game mode will not be changed.
	 *
	 * @see EPF2ModeOfPlay
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual void RequestDowntimeMode() = 0;

	/**
	 * Adds a character to the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter and the given character is already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual void AddCharacterToEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Removes a character from the current encounter, if an encounter is active.
	 *
	 * This call will have no effect under the following circumstances:
	 *	- If there is no active encounter according to game mode rules or the current Mode of Play Rule Set (MoPRS).
	 *	- If there is an active encounter but the given character is not already part of the encounter.
	 *
	 * @param Character
	 *	The character being added to the encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual void RemoveCharacterFromEncounter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Notifies game rules and/or the Mode of Play Rule Set (MoPRS) that a character wishes to perform a command.
	 *
	 * If the current mode is structured (e.g., Encounter mode), then the command will be placed into a queue of
	 * commands for the character -- preserving the order that the commands were queued -- and the command will be
	 * executed when it is the character's turn (e.g., according to initiative order). On the other hand, if the current
	 * Mode of Play allows characters to perform commands immediately, the command will not be queued and will instead
	 * be given the opportunity to run before this call returns.
	 *
	 * @param Character
	 *	The character for which the command is being queued.
	 * @param Command
	 *	The command being queued.
	 * @return
	 *	A result that indicates whether the command was queued, executed immediately, or refused.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Game Modes")
	virtual EPF2CommandExecuteOrQueueResult AttemptToExecuteOrQueueCommand(
		TScriptInterface<IPF2CharacterInterface>&        Character,
		TScriptInterface<IPF2CharacterCommandInterface>& Command) = 0;
};
