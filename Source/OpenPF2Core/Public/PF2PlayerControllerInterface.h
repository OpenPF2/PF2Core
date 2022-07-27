// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <UObject/Interface.h>

#include "PF2GameStateInterface.h"
#include "PF2QueuedActionHandle.h"

#include "GameModes/PF2ModeOfPlayType.h"

#include "PF2PlayerControllerInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2PlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Player Controllers.
 */
class OPENPF2CORE_API IPF2PlayerControllerInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Gets the character(s) that this player controller has the ability to control or possess.
	 *
	 * For a single-player game that supports parties or squads, this may include both the character that the player
	 * is actively controlling as well as any controllable character in this player's party or squad. Otherwise, this
	 * will return only a single character per controller.
	 *
	 * @return
	 *	All of the characters that this player controller can control.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetControlledCharacters() = 0;

	/**
	 * Gets the player controller that is implementing this interface.
	 *
	 * @return
	 *	This player controller, as a player controller actor.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Player Controllers")
	virtual APlayerController* ToPlayerController() = 0;

	/**
	 * Cancels an action that was previously queued for this character.
	 *
	 * @param ActionHandle
	 *	A reference to the ability that has been queued-up.
	 */
	UFUNCTION(
		BlueprintCallable,
		Server,
		Unreliable,
		DisplayName="Cancel Queued Action",
		Category="OpenPF2|Player Controllers")
	virtual void ServerCancelQueuedAction(const FPF2QueuedActionHandle ActionHandle) = 0;

	// =================================================================================================================
	// Public Event Notifications from the Game State
	// =================================================================================================================
	/**
	 * Notifies this player controller that the mode of play has changed.
	 *
	 * (This should normally be invoked only by the game state).
	 *
	 * @see EPF2ModeOfPlay
	 *
	 * @param NewMode
	 *	The new mode of play.
	 */
	UFUNCTION()
	virtual void HandleModeOfPlayChanged(EPF2ModeOfPlayType NewMode) = 0;

	// =================================================================================================================
	// Public Event Notifications from Mode of Play Rule Sets (MoPRS)
	// =================================================================================================================
	/**
	 * Notifies this player controller that the pawn's turn during an encounter has started.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnStarted() = 0;

	/**
	 *  Notifies this player controller that the pawn's turn during an encounter has ended.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnEnded() = 0;

	/**
	 * Notifies this player controller that an action/ability for the character being controlled has been queued-up.
	 *
	 * This happens if the active Mode of Play Rule Set (MoPRS) is requiring characters to queue up execution of
	 * abilities until their turn to attack/act.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 *
	 * @param ActionHandle
	 *	A reference to the ability that was queued.
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleActionQueued(const FPF2QueuedActionHandle ActionHandle) = 0;

	/**
	 * Notifies this player controller a previously queued action/ability has been removed from the queue.
	 *
	 * (This should normally be invoked only by the MoPRS).
	 *
	 * @param ActionHandle
	 *	A reference to the ability that has been removed.
	 */
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleActionDequeued(const FPF2QueuedActionHandle ActionHandle) = 0;
};
