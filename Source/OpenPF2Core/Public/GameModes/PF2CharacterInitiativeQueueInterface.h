// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <CoreMinimal.h>

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CharacterInitiativeQueueInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterInitiativeQueueInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for objects that maintain queues of commands/actions.
 */
class OPENPF2CORE_API IPF2CharacterInitiativeQueueInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Determines if there are any characters in this queue.
	 *
	 * @return
	 *	- true if the queue has no characters.
	 *	- false if the queue has at least one character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual bool IsEmpty() = 0;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character,
	                                    const int32                                     Initiative) = 0;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const = 0;

	/**
	 * Clears any initiative value set for the specified character.
	 *
	 * If the character does not have any initiative set, this has no effect.
	 *
	 * @param Character
	 *	The character for which initiative is being cleared.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) = 0;

	/**
	 * Clears all initiative values set for all characters.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual void ClearInitiativeForAllCharacters() = 0;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative() = 0;

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
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Initiative Queues")
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetCharactersInInitiativeOrder() const = 0;
};
