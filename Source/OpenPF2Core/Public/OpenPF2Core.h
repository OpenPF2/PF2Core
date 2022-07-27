// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

/**
 * Log category for logic evaluated by the OpenPF2 core code.
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2Core, Log, VeryVerbose);

/**
 * Log category for logic evaluated by OpenPF2 code that executes abilities and actions.
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2CoreAbilities, Log, VeryVerbose);

/**
 * Log category for logic evaluated by OpenPF2 during encounters.
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2CoreEncounters, Log, VeryVerbose);

/**
 * Log category for character-initiative-management logic evaluated by OpenPF2, usually during encounters.
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2CoreInitiative, Log, VeryVerbose);

/**
 * Log category for logging character abilities and stats (very verbose).
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2CoreStats, Log, VeryVerbose);

/**
 * Log category for logging character abilities and stats (very verbose).
 */
OPENPF2CORE_API DECLARE_LOG_CATEGORY_EXTERN(LogPf2CoreKeyBindings, Log, VeryVerbose);

/**
 * Top-level module for responding to events for the OpenPF2 plug-in.
 */
class FOpenPF2CoreModule final : public IModuleInterface
{
public:
	// =================================================================================================================
	// Public Methods - IModuleInterface Implementation
	// =================================================================================================================
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
