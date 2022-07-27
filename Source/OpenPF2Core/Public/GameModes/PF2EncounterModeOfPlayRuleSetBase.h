// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/ScriptInterface.h>

#include "PF2CharacterInterface.h"
#include "PF2ModeOfPlayRuleSetBase.h"
#include "PF2QueuedActionInterface.h"

#include "PF2EncounterModeOfPlayRuleSetBase.generated.h"

/**
 * Base class for PF2 Mode of Play Rule Sets (MoPRS) that need encounter logic, including initiative and queued actions.
 *
 * Encounters often involve each character "rolling" for their initiative order, and then performing attacks in order
 * from highest to lowest initiative score. This class provides several convenience methods for Blueprint sub-classes to
 * set initiatives for characters and then iterate through characters in order of their initiative.
 *
 * Since OpenPF2 is intended to support action RPGs just as well as turn-based RPGs, this base class also provides
 * convenience methods to support the ability for characters to "queue-up" abilities/actions that they want to perform
 * when it is their turn. This gives Blueprint sub-classes full control over how they want to implement combat, either
 * allowing each character to act one-by-one; or, cycling through characters at a rapid clip to keep combat flowing
 * despite the turn-based nature of PF2 rules.
 */
UCLASS(Abstract, Blueprintable)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2EncounterModeOfPlayRuleSetBase : public UPF2ModeOfPlayRuleSetBase
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Internal Classes
	// =================================================================================================================
	/**
	 * Internal structure used for associating a queued action handle with the character and action for it.
	 */
	struct FPF2QueuedActionHandleDetails
	{
		/**
		 * The handle itself.
		 */
		const FPF2QueuedActionHandle Handle;

		/**
		 * The character for which the action was queued.
		 */
		const TWeakInterfacePtr<IPF2CharacterInterface> Character;

		/**
		 * The action that was queued.
		 */
		const TWeakInterfacePtr<IPF2QueuedActionInterface> Action;

		/**
		 * Constructor for FPF2QueuedActionHandleDetails.
		 */
		FPF2QueuedActionHandleDetails(const FPF2QueuedActionHandle     Handle,
		                              const IPF2CharacterInterface*    Character,
		                              const IPF2QueuedActionInterface* Action) :
			Handle(Handle),
			Character(const_cast<IPF2CharacterInterface*>(Character)),
			Action(const_cast<IPF2QueuedActionInterface*>(Action))
		{
		}
	};

	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
	/**
	 * The queue of actions for each character in the encounter.
	 */
	TMultiMap<IPF2CharacterInterface*, IPF2QueuedActionInterface *> CharacterQueues;

	/**
	 * A map of initiative to characters.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 1: Roll Initiative":
	 * "If your result is tied with a foe’s result, the adversary goes first. If your result is tied with another PC’s,
	 * you can decide between yourselves who goes first when you reach that place in the initiative order. After that,
	 * your places in the initiative order usually don’t change during the encounter."
	 */
	TMultiMap<int, IPF2CharacterInterface*> CharactersByInitiatives;

	/**
	 * All of the characters in the initiative, ordered from highest to lowest initiative.
	 *
	 * This array is rebuilt whenever characters are added or removed from the encounter.
	 */
	TArray<IPF2CharacterInterface*> CurrentCharacterSequence;

	/**
	 * The last character that was returned by GetNextCharacterByInitiative().
	 */
	IPF2CharacterInterface* PreviousCharacter;

	/**
	 * The initiative of the last character that was returned by GetNextCharacterByInitiative().
	 */
	int32 PreviousCharacterIndex;

	/**
	 * A look-up from handle IDs to information about issued handles.
	 */
	TMap<int32, FPF2QueuedActionHandleDetails> IssuedActionHandles;

	/**
	 * A look-up from queued actions to the handles that reference them.
	 */
	TMap<const IPF2QueuedActionInterface*, const FPF2QueuedActionHandle> ActionHandles;

	/**
	 * The next ID to assign to an action handle.
	 */
	int32 NextActionHandleId;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2EncounterModeOfPlayRuleSetBase.
	 */
	explicit UPF2EncounterModeOfPlayRuleSetBase() :
		PreviousCharacter(nullptr),
		PreviousCharacterIndex(-1),
		NextActionHandleId(0)
	{
	}

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Signals the start of the specified character's turn.
	 *
	 * @param Character
	 *	The character for whom a turn is starting.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Turns")
	void StartTurnForCharacter(const TScriptInterface<IPF2CharacterInterface> Character);

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
	 * Adds the specified action to the queue of actions being maintained for the specified PF2 character.
	 *
	 * @param Character
	 *	The character that is queuing the action up.
	 * @param Action
	 *	The action that is being queued.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	FPF2QueuedActionHandle QueueActionForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&    Character,
		const TScriptInterface<IPF2QueuedActionInterface>& Action);

	/**
	 * Removes the specified action from the queue of actions being maintained for the specified PF2 character.
	 *
	 * @param ActionHandle
	 *	A reference to the previously-queued action that is being canceled.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	void RemoveQueuedActionForCharacterByHandle(const FPF2QueuedActionHandle ActionHandle);

	/**
	 * Removes the specified action from the queue of actions being maintained for the specified PF2 character.
	 *
	 * @param Character
	 *	The character that queued-up the action.
	 * @param Action
	 *	The previously-queued action that is being canceled.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	void RemoveQueuedActionForCharacter(
		const TScriptInterface<IPF2CharacterInterface>&    Character,
		const TScriptInterface<IPF2QueuedActionInterface>& Action);

	/**
	 * Cancels and clears all actions queued for all characters.
	 *
	 * This is typically fired right before a transition out of an encounter.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	void CancelQueuedActionsForAllCharacters();

	/**
	 * Performs the next action in the specified character's queue of actions (if there is one).
	 *
	 * The action is automatically removed from the character's queue of actions.
	 *
	 * @param Character
	 *	The character whose turn it is to act.
	 *
	 * @return
	 *	- EPF2AbilityActivationResult::None if the character is out of actions to execute.
	 *	- EPF2AbilityActivationResult::Activated if the character had an action queued up that was executed.
	 *	- EPF2AbilityActivationResult::Blocked if the character has an action to execute but it cannot be executed yet,
	 *	  typically because it is blocked by another ability that is active on the character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	UPARAM(DisplayName = "Activation Result")
	EPF2AbilityActivationResult ExecuteNextQueuedActionForCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character);

	/**
	 * Returns whether there is another action queued for the specified character.
	 *
	 * @param Character
	 *	The character for which actions will be inspected.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	bool DoesCharacterHaveNextActionQueued(const TScriptInterface<IPF2CharacterInterface>& Character) const;

	/**
	 * Returns the next action in the specified character's queue of actions (if there is one).
	 *
	 * The action is not removed from the queue.
	 *
	 * @param Character
	 *	The character for which an action is desired.
	 * @param NextAction
	 *	The output -- either the next action for the specified character; or an invalid reference if the character does
	 *	not have any more queued actions.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	void PeekNextQueuedActionForCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		TScriptInterface<IPF2QueuedActionInterface>&    NextAction) const;

	/**
	 * Removes and returns the next action in the specified character's queue of actions (if there is one).
	 *
	 * The action is removed from the queue.
	 *
	 * @param Character
	 *	The character for which an action is desired.
	 * @param NextAction
	 *	The output -- either the next action for the specified character; or an invalid reference if the character does
	 *	not have any more queued actions.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Mode of Play Rule Sets|Action Queue")
	void PopNextActionQueuedForCharacter(
		const TScriptInterface<IPF2CharacterInterface>& Character,
		TScriptInterface<IPF2QueuedActionInterface>&    NextAction);

	/**
	 * Rebuilds the sequence of characters according to initiative order.
	 *
	 * All characters are sorted from highest to lowest initiative order in the rebuilt list. If two characters have the
	 * same initiative, their order will be adjusted so that one goes before the other. Per PF2 rules (see below),
	 * Playable Characters (PCs) with the same initiative as Non-Playable Characters (NPCs) are sorted after NPCs so
	 * that NPCs take turns first. Unlike with standard PF2 rules, though, if multiple characters of the same type --
	 * either two PCs or two NPCs -- we randomize their order rather than give each character a choice of preferred
	 * order. This helps to keep combat fluid by avoiding having to prompt players for input at the start of encounters.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 13, "Initiative":
	 * "At the start of an encounter, all creatures involved roll for initiative to determine the order in which they
	 * act. The higher the result of its roll, the earlier a creature gets to act."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 468, "Step 1: Roll Initiative":
	 * "If your result is tied with a foe’s result, the adversary goes first. If your result is tied with another PC’s,
	 * you can decide between yourselves who goes first when you reach that place in the initiative order. After that,
	 * your places in the initiative order usually don’t change during the encounter."
	 *
	 * This method performs a lot of sorting and is expensive. It should only be called when the list of characters has
	 * changed.
	 */
	void RebuildCharacterSequence();

	/**
	 * Attempts to locate the specified character in the initiative map and then remove them.
	 *
	 * This method does not rebuild the character sequence. In most cases, you will want to use
	 * ClearInitiativeForCharacter() instead, as that method calls this method and then rebuilds the character sequence.
	 *
	 * @param Character
	 *	The character being removed from the map.
	 */
	void RemoveCharacterFromInitiativeMap(const IPF2CharacterInterface* Character);
};
