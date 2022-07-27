// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>
#include <Engine/Texture2D.h>

#include "Commands/PF2CommandExecuteImmediatelyResult.h"
#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CharacterCommandInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterCommandInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for commands that can either be executed immediately, or defer their execution to a future time.
 */
class OPENPF2CORE_API IPF2CharacterCommandInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Utility function for converting a EPF2CommandExecuteImmediatelyResult into a EPF2CommandExecuteOrQueueResult.
	 *
	 * @param ImmediateResult
	 *	The result to transcribed.
	 *
	 * @return
	 *	The transcribed result.
	 */
	FORCEINLINE static EPF2CommandExecuteOrQueueResult ImmediateResultToExecuteOrQueueResult(
		const EPF2CommandExecuteImmediatelyResult ImmediateResult)
	{
		switch (ImmediateResult)
		{
			default:
			case EPF2CommandExecuteImmediatelyResult::None:
				return EPF2CommandExecuteOrQueueResult::None;

			case EPF2CommandExecuteImmediatelyResult::Activated:
				return EPF2CommandExecuteOrQueueResult::ExecutedImmediately;

			case EPF2CommandExecuteImmediatelyResult::Blocked:
				return EPF2CommandExecuteOrQueueResult::Refused;

			case EPF2CommandExecuteImmediatelyResult::Cancelled:
				return EPF2CommandExecuteOrQueueResult::Refused;
		}
	}

	// =================================================================================================================
	// Public Instance Methods
	// =================================================================================================================
	/**
	 * Gets an icon to represent this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A graphical representation of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual UTexture2D* GetCommandIcon() const = 0;

	/**
	 * Gets the name of this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A short textual description of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual FText GetCommandLabel() const = 0;

	/**
	 * Gets the description of this command, for whenever it is displayed to players/users.
	 *
	 * @return
	 *	A long textual description of this command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual FText GetCommandDescription() const = 0;

	/**
	 * Attempt to execute this command immediately, if possible; queue it for the character, if not possible.
	 *
	 * This method only has an effect on the server. If it is called on a client, the result will be
	 * EPF2CommandExecuteOrQueueResult::None. In rare circumstances, it is possible that the command is neither queued
	 * nor executed, in which case the result will be EPF2CommandExecuteOrQueueResult::Refused.
	 *
	 * @return
	 *	The outcome of attempting to execute the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual EPF2CommandExecuteOrQueueResult AttemptExecuteOrQueue() = 0;

	/**
	 * Attempt to execute this command immediately, if possible; do nothing, if not possible.
	 *
	 * This method only has an effect on the server. If it is called on a client, the result will be
	 * EPF2CommandExecuteImmediatelyResult::None.
	 *
	 * @return
	 *	The outcome of attempting to execute the command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual EPF2CommandExecuteImmediatelyResult AttemptExecuteImmediately() = 0;

	/**
	 * Cancel this command, removing it from the owning character's queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	virtual void Cancel() = 0;
};
