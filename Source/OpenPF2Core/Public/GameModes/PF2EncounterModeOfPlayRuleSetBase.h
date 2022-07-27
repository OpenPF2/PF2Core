// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/ScriptInterface.h>

#include "PF2CharacterInitiativeQueueInterface.h"
#include "PF2ModeOfPlayRuleSetBase.h"

#include "Commands/PF2CommandExecuteImmediatelyResult.h"

#include "Utilities/PF2InterfaceUtilities.h"

#include "PF2EncounterModeOfPlayRuleSetBase.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * Base class for PF2 Mode of Play Rule Sets (MoPRS) that need encounter logic, including initiative and command queues.
 *
 * Encounters often involve each character "rolling" for their initiative order, and then performing attacks in order
 * from highest to lowest initiative score. This class provides several convenience methods for Blueprint sub-classes to
 * set initiatives for characters and then iterate through characters in order of their initiative.
 *
 * Since OpenPF2 is intended to support action RPGs just as well as turn-based RPGs, this base class also provides
 * convenience methods to support the ability for characters to "queue-up" abilities/commands that they want to perform
 * when it is their turn. This gives Blueprint sub-classes full control over how they want to implement combat, either
 * allowing each character to act one-by-one; or, cycling through characters at a rapid clip to keep combat flowing
 * despite the turn-based nature of PF2 rules.
 */
UCLASS(Abstract, Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2EncounterModeOfPlayRuleSetBase : public APF2ModeOfPlayRuleSetBase
{
	GENERATED_BODY()

protected:
	/**
	 * The component of the MoPRS that maintains the list of characters and their initiatives.
	 */
	IPF2CharacterInitiativeQueueInterface* CharacterInitiativeQueue;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for APF2EncounterModeOfPlayRuleSetBase.
	 */
	explicit APF2EncounterModeOfPlayRuleSetBase();

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the component of the MoPRS that maintains the list of characters and their initiatives.
	 *
	 * @return
	 *	The character initiative queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	FORCEINLINE TScriptInterface<IPF2CharacterInitiativeQueueInterface> GetCharacterInitiativeQueue() const
	{
		return PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInitiativeQueueInterface>(
			this->CharacterInitiativeQueue
		);
	}

	/**
	 * Gets whether there are payable characters with initiative set in this encounter.
	 *
	 * @return
	 *	- true if there is at least one playable character who is not dead and who has initiative set.
	 *	- false if there are no playable, living characters with initiative set.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Characters")
	bool HavePlayableCharacters() const;

	/**
	 * Signals the start of the specified character's turn.
	 *
	 * @param Character
	 *	The character for whom a turn is starting.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Turns")
	void StartTurnForCharacter(const TScriptInterface<IPF2CharacterInterface> Character) const;

	/**
	 * Signals the end of the specified character's turn.
	*
	 * @param Character
	 *	The character for whom a turn is ending.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Turns")
	void EndTurnForCharacter(const TScriptInterface<IPF2CharacterInterface> Character);

	/**
	 * Sets the initiative of the specified character.
	 *
	 * If the character already has an initiative set, the character's initiative is changed to the specified value.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * @param Character
	 *	The character for which initiative is being set.
	 * @param Initiative
	 *	The initiative value to use for the specified character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character, const int32 Initiative);

	/**
	 * Determines if the specified character has an initiative set.
	 *
	 * @param Character
	 *	The character for which initiative is being checked.
	 *
	 * @return
	 *	- true if the character has an initiative set.
	 *	- false if the character does not have any initiative set.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const;

	/**
	 * Clears any initiative value set for the specified character.
	 *
	 * If the character does not have any initiative set, this has no effect.
	 *
	 * @param Character
	 *	The character for which initiative is being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Clears all initiative values set for all characters.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	void ClearInitiativeForAllCharacters();

	/**
	 * Gets the next character in initiative order, looping around to the first character when at the end of the list.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 2: Play a Round":
	 * "A round begins when the participant with the highest initiative roll result starts their turn, and it ends when
	 * the one with the lowest initiative ends their turn."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 3: Begin the Next Round":
	 * "Once everyone in the encounter has taken a turn, the round is over and the next one begins. Don’t roll
	 * initiative again; the new round proceeds in the same order as the previous one, repeating the cycle until the
	 * encounter ends."
	 *
	 * This method maintains an internal iterator in this rule set. If initiative is set, changed, or cleared for
	 * characters in the encounter after iteration has begun, the iterator can make only the following two guarantees
	 * about the next character to be returned relative to the character that was returned the last time this method was
	 * called:
	 * 1. The next character will be a different character than the last character, unless there is only one character
	 *    in the encounter.
	 * 2. The next character will have either a lower initiative or an equal initiative to the last character, unless
	 *    the last character returned was the character in the encounter with the lowest initiative.
	 *
	 * Only characters that have an initiative set are returned; all others are ignored.
	 *
	 * @return
	 *	The next character in the encounter who has an initiative equal to or lower than the character that was last
	 *	returned; or, if at the end of the list of characters, the character with the highest initiative in the
	 *	encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative();

	/**
	 * Gets all characters in the order of their initiative.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * Only characters that have an initiative set are returned; all others are excluded.
	 *
	 * @return
	 *	The list of characters, sorted in order of highest to lowest initiative.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Initiative")
	TArray<TScriptInterface<IPF2CharacterInterface>> GetCharactersInInitiativeOrder() const;

	/**
	 * Adds the specified command to the queue of commands being maintained for the specified PF2 character.
	 *
	 * @param Character
	 *	The character that is queuing the command up.
	 * @param Command
	 *	The command that is being queued.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	void QueueCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&        Character,
		const TScriptInterface<IPF2CharacterCommandInterface>& Command) const;

	/**
	 * Cancels and clears all commands queued for all characters.
	 *
	 * This is typically fired right before a transition out of an encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	void CancelQueuedCommandsForAllCharacters();

	/**
	 * Performs the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is automatically removed from the character's queue of commands.
	 *
	 * @param Character
	 *	The character whose turn it is to act.
	 *
	 * @return
	 *	- EPF2ImmediateCommandExecutionResult::None if the character is out of commands to execute.
	 *	- EPF2ImmediateCommandExecutionResult::Activated if the character had an command queued up that was executed.
	 *	- EPF2ImmediateCommandExecutionResult::Blocked if the character has an command to execute but it cannot be
	 *	  executed yet, typically because it is blocked by another ability that is active on the character.
	 *	- EPF2ImmediateCommandExecutionResult::Cancelled if the rule set has opted to cancel the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	UPARAM(DisplayName = "Activation Result")
	EPF2CommandExecuteImmediatelyResult ExecuteNextQueuedCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Returns whether there is another command queued for the specified character.
	 *
	 * @param Character
	 *	The character for which commands will be inspected.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	bool DoesCharacterHaveNextCommandQueued(const TScriptInterface<IPF2CharacterInterface>& Character) const;

	/**
	 * Returns the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is not removed from the queue.
	 *
	 * @param Character
	 *	The character for which an command is desired.
	 * @param NextCommand
	 *	The output -- either the next command for the specified character; or an invalid reference if the character does
	 *	not have any more queued commands.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	void PeekNextQueuedCommandForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) const;

	/**
	 * Removes and returns the next command in the specified character's queue of commands (if there is one).
	 *
	 * The command is removed from the queue.
	 *
	 * @param Character
	 *	The character for which an command is desired.
	 * @param NextCommand
	 *	The output -- either the next command for the specified character; or an invalid reference if the character does
	 *	not have any more queued commands.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Command Queue")
	void PopNextCommandQueuedForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&  Character,
		TScriptInterface<IPF2CharacterCommandInterface>& NextCommand);
};
