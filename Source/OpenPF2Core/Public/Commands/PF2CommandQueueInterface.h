// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>

#include "PF2CommandExecuteImmediatelyResult.h"
#include "Utilities/PF2LogIdentifiableInterface.h"

#include "PF2CommandQueueInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
class IPF2CharacterCommandInterface;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CommandQueueInterface : public UPF2LogIdentifiableInterface
{
	GENERATED_BODY()
};

/**
 * An interface for objects that maintain queues of commands/actions.
 */
class OPENPF2CORE_API IPF2CommandQueueInterface : public IPF2LogIdentifiableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Adds a command to the end of the queue.
	 *
	 * @param Command
	 *	The command to add to the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual void Enqueue(TScriptInterface<IPF2CharacterCommandInterface> Command) = 0;

	/**
	 * Returns the next command (if there is one) in the queue.
	 *
	 * The command is not removed from the queue.
	 *
	 * @param NextCommand
	 *	The output: either the next command in the queue; or an invalid reference if there are no commands in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual void PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) = 0;

	/**
	 * Removes and returns the first/oldest command (if there is one) from the queue.
	 *
	 * The command is removed from the queue.
	 *
	 * @param NextCommand
	 *	The output: either the next command in the queue; or an invalid reference if there are no commands in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual void PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) = 0;

	/**
	 * Removes the first/oldest command (if there is one) from the queue and drops it.
	 *
	 * The command is removed from the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual void DropNext() = 0;

	/**
	 * Removes and executes the first/oldest command (if there is one) from the queue.
	 *
	 * The command is removed from the queue.
	 *
	* @return
	 *	- EPF2ImmediateCommandExecutionResult::None if there are no commands to execute.
	 *	- EPF2ImmediateCommandExecutionResult::Activated if there was a queued command that was executed.
	 *	- EPF2ImmediateCommandExecutionResult::Blocked if there was a queued command but it could not be executed yet,
	 *	  typically because it is blocked by another ability that is active on the character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual EPF2CommandExecuteImmediatelyResult PopAndExecuteNext() = 0;

	/**
	 * Removes a command from the queue.
	 *
	 * If the command is not in the queue, nothing happens (this method is idempotent).
	 *
	 * @param Command
	 *	The command to remove, if it exists in the queue.
	 *
	 * @return
	 *	- true if the command was removed from the queue.
	 *	- false if the command was not in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual bool Remove(TScriptInterface<IPF2CharacterCommandInterface> Command) = 0;

	/**
	 * Gets the count of how many commands are in the queue.
	 *
	 * @return
	 *	How many items are in the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual int Count() = 0;

	/**
	 * Removes all commands from the queue.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Command Queues")
	virtual void Clear() = 0;
};
