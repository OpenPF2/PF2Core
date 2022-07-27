// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "PF2CommandExecuteImmediatelyResult.generated.h"

/**
 * An enumeration of the different outcomes from attempting to execute a command immediately.
 */
UENUM(BlueprintType)
enum class EPF2CommandExecuteImmediatelyResult : uint8
{
	/**
	 * No result yet.
	 */
	None,

	/**
	 * The command was successfully executed/performed.
	 *
	 * The command should be de-queued.
	 */
	Activated,

	/**
	 * The command was not performed because one of its requirements (e.g., tags, cost, etc.) was not satisfied.
	 *
	 * The command should be re-queued, if possible.
	 */
	Blocked,

	/**
	 * The command was not performed because it was canceled externally.
	 */
	Cancelled,
};
