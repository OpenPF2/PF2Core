// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2CommandExecuteOrQueueResult.generated.h"

/**
 * An enumeration of the different outcomes from attempting to execute a command or queue it for later.
 */
UENUM(BlueprintType)
enum class EPF2CommandExecuteOrQueueResult : uint8
{
	/**
	 * No result yet.
	 */
	None,

	/**
	 * The command did not need to be queued and was executed immediately.
	 */
	ExecutedImmediately,

	/**
	 * The command was not executed immediately but was successfully queued.
	 */
	Queued,

	/**
	 * The command was neither queued nor executed and should be removed from any queues and canceled/ended.
	 */
	Refused,
};
