// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameModes/PF2ModeOfPlayRuleSetBase.h"

#include "Libraries/PF2CharacterLibrary.h"

EPF2CommandExecuteOrQueueResult APF2ModeOfPlayRuleSetBase::AttemptToExecuteOrQueueCommand_Implementation(
	const TScriptInterface<IPF2CharacterInterface>&        Character,
	const TScriptInterface<IPF2CharacterCommandInterface>& Command)
{
	EPF2CommandExecuteOrQueueResult Result;

	// By default there is no queue, so we perform the command immediately.
	Result = IPF2CharacterCommandInterface::ImmediateResultToExecuteOrQueueResult(Command->AttemptExecuteImmediately());

	return Result;
}

TScriptInterface<IPF2GameModeInterface> APF2ModeOfPlayRuleSetBase::GetGameMode() const
{
	AGameModeBase* GameMode = this->GetWorld()->GetAuthGameMode();

	check(GameMode);

	return TScriptInterface<IPF2GameModeInterface>(GameMode);
}

TArray<TScriptInterface<IPF2PlayerControllerInterface>> APF2ModeOfPlayRuleSetBase::GetPlayerControllers() const
{
	return UPF2CharacterLibrary::GetPlayerControllers(this->GetWorld());
}

TArray<TScriptInterface<IPF2CharacterInterface>> APF2ModeOfPlayRuleSetBase::GetPlayerControlledCharacters() const
{
	return UPF2CharacterLibrary::GetPlayerControlledCharacters(this->GetWorld());
}
