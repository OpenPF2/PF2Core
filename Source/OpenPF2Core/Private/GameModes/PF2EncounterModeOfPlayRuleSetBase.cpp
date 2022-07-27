// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GameModes/PF2EncounterModeOfPlayRuleSetBase.h"

#include "OpenPF2Core.h"
#include "PF2CharacterInterface.h"
#include "PF2PlayerControllerBase.h"
#include "PF2PlayerControllerInterface.h"

#include "Commands/PF2CommandQueueInterface.h"

#include "GameModes/PF2CharacterInitiativeQueueComponent.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

APF2EncounterModeOfPlayRuleSetBase::APF2EncounterModeOfPlayRuleSetBase()
{
	this->CharacterInitiativeQueue =
		this->CreateDefaultSubobject<UPF2CharacterInitiativeQueueComponent>(TEXT("CharacterInitiativeQueue"));
}

bool APF2EncounterModeOfPlayRuleSetBase::HavePlayableCharacters() const
{
	bool Result = false;

	if (!this->GetCharacterInitiativeQueue()->IsEmpty())
	{
		for (const TScriptInterface<IPF2CharacterInterface> Character : this->GetCharactersInInitiativeOrder())
		{
			if (Character->IsAlive())
			{
				Result = true;
				break;
			}
		}
	}

	return Result;
}

void APF2EncounterModeOfPlayRuleSetBase::StartTurnForCharacter(
	const TScriptInterface<IPF2CharacterInterface> Character) const
{
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	check(Character != nullptr);

	UE_LOG(
		LogPf2CoreEncounters,
		Verbose,
		TEXT("[%s] Starting turn for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Character->GetIdForLogs())
	);

	if (PlayerController != nullptr)
	{
		IPF2PlayerControllerInterface::Execute_MulticastHandleEncounterTurnStarted(PlayerController.GetObject());
	}

	IPF2CharacterInterface::Execute_MulticastHandleEncounterTurnStarted(Character.GetObject());
}

void APF2EncounterModeOfPlayRuleSetBase::EndTurnForCharacter(const TScriptInterface<IPF2CharacterInterface> Character)
{
	const TScriptInterface<IPF2PlayerControllerInterface> PlayerController = Character->GetPlayerController();

	check(Character != nullptr);

	UE_LOG(
		LogPf2CoreEncounters,
		Verbose,
		TEXT("[%s] Ending turn for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Character->GetIdForLogs())
	);

	if (PlayerController != nullptr)
	{
		IPF2PlayerControllerInterface::Execute_MulticastHandleEncounterTurnEnded(PlayerController.GetObject());
	}

	IPF2CharacterInterface::Execute_MulticastHandleEncounterTurnEnded(Character.GetObject());
}

void APF2EncounterModeOfPlayRuleSetBase::SetCharacterInitiative(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	const int32                                     Initiative)
{
}

bool APF2EncounterModeOfPlayRuleSetBase::IsInitiativeSetForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	return this->GetCharacterInitiativeQueue()->IsInitiativeSetForCharacter(Character);
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void APF2EncounterModeOfPlayRuleSetBase::ClearInitiativeForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	this->GetCharacterInitiativeQueue()->ClearInitiativeForCharacter(Character);
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
void APF2EncounterModeOfPlayRuleSetBase::ClearInitiativeForAllCharacters()
{
	this->GetCharacterInitiativeQueue()->ClearInitiativeForAllCharacters();
}

// ReSharper disable once CppUE4BlueprintCallableFunctionMayBeConst
TScriptInterface<IPF2CharacterInterface> APF2EncounterModeOfPlayRuleSetBase::GetNextCharacterByInitiative()
{
	return this->GetCharacterInitiativeQueue()->GetNextCharacterByInitiative();
}

TArray<TScriptInterface<IPF2CharacterInterface>> APF2EncounterModeOfPlayRuleSetBase::GetCharactersInInitiativeOrder() const
{
	return this->GetCharacterInitiativeQueue()->GetCharactersInInitiativeOrder();
}

void APF2EncounterModeOfPlayRuleSetBase::QueueCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>&        Character,
	const TScriptInterface<IPF2CharacterCommandInterface>& Command) const
{
	const IPF2CharacterInterface* CharacterIntf = PF2InterfaceUtilities::FromScriptInterface(Character);

	check(CharacterIntf != nullptr);

	UE_LOG(
		LogPf2CoreEncounters,
		VeryVerbose,
		TEXT("[%s] Queuing command ('%s') for character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(Command->GetIdForLogs()),
		*(Character->GetIdForLogs())
	);

	CharacterIntf->GetCommandQueueComponent()->Enqueue(Command);
}

void APF2EncounterModeOfPlayRuleSetBase::CancelQueuedCommandsForAllCharacters()
{
	for (const TScriptInterface<IPF2CharacterInterface> Character : this->GetCharactersInInitiativeOrder())
	{
		Character->GetCommandQueueComponent()->Clear();
	}
}

EPF2CommandExecuteImmediatelyResult APF2EncounterModeOfPlayRuleSetBase::ExecuteNextQueuedCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character)
{
	return Character->GetCommandQueueComponent()->PopAndExecuteNext();
}

bool APF2EncounterModeOfPlayRuleSetBase::DoesCharacterHaveNextCommandQueued(
	const TScriptInterface<IPF2CharacterInterface>& Character) const
{
	TScriptInterface<IPF2CharacterCommandInterface> NextCommand;

	Character->GetCommandQueueComponent()->PeekNext(NextCommand);

	return (NextCommand != nullptr);
}

void APF2EncounterModeOfPlayRuleSetBase::PeekNextQueuedCommandForCharacter(
	const TScriptInterface<IPF2CharacterInterface>&  Character,
	TScriptInterface<IPF2CharacterCommandInterface>& NextCommand) const
{
	Character->GetCommandQueueComponent()->PeekNext(NextCommand);
}

void APF2EncounterModeOfPlayRuleSetBase::PopNextCommandQueuedForCharacter(
	const TScriptInterface<IPF2CharacterInterface>& Character,
	TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	Character->GetCommandQueueComponent()->PopNext(NextCommand);
}
