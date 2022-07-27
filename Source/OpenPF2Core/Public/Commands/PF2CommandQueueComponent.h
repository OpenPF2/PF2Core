// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <Containers/CircularQueue.h>

#include "PF2CommandQueueInterface.h"

#include "Commands/PF2CharacterCommandInterface.h"

#include "PF2CommandQueueComponent.generated.h"

// =====================================================================================================================
// Delegate Types
// =====================================================================================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2CommandAddedToQueueDelegate,
	const TScriptInterface<IPF2CharacterCommandInterface>&,
	Command
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2CommandRemovedFromQueueDelegate,
	const TScriptInterface<IPF2CharacterCommandInterface>&,
	Command
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPF2CommandQueueChangedDelegate,
	const TArray<TScriptInterface<IPF2CharacterCommandInterface>>&,
	Commands
);

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A component for managing commands/actions that have been queued for a particular character.
 *
 * (The inspiration for this approach -- of wrapping Gameplay Abilities in a command that can be invoked at any time --
 * came from "ue4-orders-abilities" by Daedalic Entertainment,
 * https://github.com/DaedalicEntertainment/ue4-orders-abilities).
 */
UCLASS(ClassGroup="OpenPF2", meta=(BlueprintSpawnableComponent))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2CommandQueueComponent : public UActorComponent, public IPF2CommandQueueInterface
{
	GENERATED_BODY()

protected:
	TArray<IPF2CharacterCommandInterface*> Queue;

public:
	/**
	 * Event fired when a command has been added to this queue.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2CommandAddedToQueueDelegate OnCommandAdded;

	/**
	 * Event fired when a command has been removed from this queue.
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2CommandRemovedFromQueueDelegate OnCommandRemoved;

	/**
	 * Event fired when the commands in the queue have changed (commands added, commands removed, or queue cleared).
	 */
	UPROPERTY(BlueprintAssignable)
	FPF2CommandQueueChangedDelegate OnCommandsChanged;

	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor for UPF2CommandQueueComponent.
	 */
	UPF2CommandQueueComponent();

	// =================================================================================================================
	// Public Methods - IPF2CommandQueueInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual void Enqueue(TScriptInterface<IPF2CharacterCommandInterface> Command) override;

	UFUNCTION(BlueprintCallable)
	virtual void PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) override;

	UFUNCTION(BlueprintCallable)
	virtual void PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) override;

	UFUNCTION(BlueprintCallable)
	virtual void DropNext() override;

	UFUNCTION(BlueprintCallable)
	virtual EPF2CommandExecuteImmediatelyResult PopAndExecuteNext() override;

	UFUNCTION(BlueprintCallable)
	virtual bool Remove(TScriptInterface<IPF2CharacterCommandInterface> Command) override;

	UFUNCTION(BlueprintCallable)
	virtual int Count() override;

	UFUNCTION(BlueprintCallable)
	virtual void Clear() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual FString GetIdForLogs() const override;
};
