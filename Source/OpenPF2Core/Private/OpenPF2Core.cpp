// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "OpenPF2Core.h"

#define LOCTEXT_NAMESPACE "FOpenPF2CoreModule"

void FOpenPF2CoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin
	// file per-module
}

void FOpenPF2CoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FOpenPF2CoreModule, OpenPF2Core)

/** Logging definitions */
DEFINE_LOG_CATEGORY(LogPf2Core);
DEFINE_LOG_CATEGORY(LogPf2CoreAbilities);
DEFINE_LOG_CATEGORY(LogPf2CoreEncounters);
DEFINE_LOG_CATEGORY(LogPf2CoreInitiative);
DEFINE_LOG_CATEGORY(LogPf2CoreStats);
DEFINE_LOG_CATEGORY(LogPf2CoreKeyBindings);
