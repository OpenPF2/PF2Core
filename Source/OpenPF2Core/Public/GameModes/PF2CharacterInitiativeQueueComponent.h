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
#include <Components/ActorComponent.h>

#include "PF2CharacterInitiativeQueueInterface.h"
#include "PF2CharacterInitiativeQueueComponent.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2PlayerControllerInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UCLASS(ClassGroup="OpenPF2", meta=(BlueprintSpawnableComponent))
class OPENPF2CORE_API UPF2CharacterInitiativeQueueComponent final :
	public UActorComponent,
	public IPF2CharacterInitiativeQueueInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Fields
	// =================================================================================================================
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
	 * All of the characters in the queue, ordered from highest to lowest initiative.
	 *
	 * This array is rebuilt whenever characters are added or removed from the queue.
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

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2CharacterInitiativeQueueComponent.
	 */
	explicit UPF2CharacterInitiativeQueueComponent() :
		PreviousCharacter(nullptr),
		PreviousCharacterIndex(-1)
	{
	}

	// =================================================================================================================
	// Public Methods - IPF2CharacterInitiativeQueueInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual bool IsEmpty() override;

	UFUNCTION(BlueprintCallable)
	virtual void SetCharacterInitiative(const TScriptInterface<IPF2CharacterInterface>& Character,
	                                    const int32                                     Initiative) override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsInitiativeSetForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) const override;

	UFUNCTION(BlueprintCallable)
	virtual void ClearInitiativeForCharacter(const TScriptInterface<IPF2CharacterInterface>& Character) override;

	UFUNCTION(BlueprintCallable)
	virtual void ClearInitiativeForAllCharacters() override;

	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IPF2CharacterInterface> GetNextCharacterByInitiative() override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetCharactersInInitiativeOrder() const override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets controllers for all the characters in the world.
	 *
	 * @return
	 *	PF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable)
	TArray<TScriptInterface<IPF2PlayerControllerInterface>> GetPlayerControllers() const;

	/**
	 * Gets all of the player-controlled PF2 characters in the world.
	 *
	 * @return
	 *	PF2-specific controllers for all players in the world.
	 */
	UFUNCTION(BlueprintCallable)
	TArray<TScriptInterface<IPF2CharacterInterface>> GetPlayerControlledCharacters() const;

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
