// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameFramework/PlayerController.h>
#include <UObject/WeakInterfacePtr.h>

#include "PF2PlayerControllerInterface.h"
#include "PF2QueuedActionHandle.h"

#include "PF2PlayerControllerBase.generated.h"

/**
 * Default base class for PF2 Player Controllers.
 *
 * @see IPF2PlayerControllerInterface
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2PlayerControllerBase : public APlayerController, public IPF2PlayerControllerInterface
{
	GENERATED_BODY()

protected:
	/**
	 * The characters that can be controlled by this player controller.
	 *
	 * Depending on the game, this may represent this player's "party" or "squad".
	 */
	TArray<TWeakInterfacePtr<IPF2CharacterInterface>> ControlledCharacters;

public:
	// =================================================================================================================
	// Public Methods - AController Overrides
	// =================================================================================================================
	virtual void SetPawn(APawn* NewPawn) override;

	// =================================================================================================================
	// Public Methods - IPF2PlayerControllerInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual TArray<TScriptInterface<IPF2CharacterInterface>> GetControlledCharacters() override;

	UFUNCTION(BlueprintCallable)
	virtual APlayerController* ToPlayerController() override;

	UFUNCTION()
	virtual void HandleModeOfPlayChanged(EPF2ModeOfPlayType NewMode) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnStarted() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleEncounterTurnEnded() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleActionQueued(const FPF2QueuedActionHandle ActionHandle) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleActionDequeued(const FPF2QueuedActionHandle ActionHandle) override;

	UFUNCTION(BlueprintCallable, Server, Unreliable, DisplayName="Cancel Queued Action")
	virtual void ServerCancelQueuedAction(const FPF2QueuedActionHandle ActionHandle) override;

protected:
	// =================================================================================================================
	// Blueprint Event Callbacks
	// =================================================================================================================
	/**
	 * BP event invoked when the mode of play has changed.
	 *
	 * This is invoked on both the owning client and server.
	 *
	 * @param NewMode
	 *	The new mode of play.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Player Controllers")
	void OnModeOfPlayChanged(EPF2ModeOfPlayType NewMode);

	/**
	 * BP event invoked when the pawn's turn during an encounter has started.
	 *
	 * This is invoked on both the owning client and server.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Player Controllers")
	void OnEncounterTurnStarted();

	/**
	 * BP event invoked when the pawn's turn during an encounter has ended.
	 *
	 * This is invoked on both the owning client and server.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Player Controllers")
	void OnEncounterTurnEnded();

	/**
	 * BP event invoked when an action/ability has been queued-up for the controlled character.
	 *
	 * This is invoked on both the owning client and server.
	 *
	 * @param ActionHandle
	 *	A reference to the ability that has been queued-up.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Player Controllers")
	void OnActionQueued(const FPF2QueuedActionHandle ActionHandle);

	/**
	 * BP event invoked when a previously queued action/ability for the controlled character has been cancelled.
	 *
	 * This is invoked on both the owning client and server.
	 *
	 * This happens if an action queued through the active Mode of Play Rule Set (MoPRS) was canceled by the player,
	 * by game rules, or something in the world.
	 *
	 * @param ActionHandle
	 *	A reference to the ability that has been canceled.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="OpenPF2|Player Controllers")
	void OnActionDequeued(const FPF2QueuedActionHandle ActionHandle);
};
