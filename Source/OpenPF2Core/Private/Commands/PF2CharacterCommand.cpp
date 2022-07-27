// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CharacterCommand.h"

#include <Net/UnrealNetwork.h>

#include "Abilities/PF2GameplayAbilityInterface.h"

#include "Commands/PF2CommandQueueInterface.h"

#include "GameModes/PF2GameModeInterface.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

IPF2CharacterCommandInterface* APF2CharacterCommand::Create(AActor*                          CharacterActor,
                                                            const FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	UWorld*               World           = CharacterActor->GetWorld();
	FActorSpawnParameters SpawnParameters;
	APF2CharacterCommand* Command;

	check(CharacterActor->Implements<UPF2CharacterInterface>());

	SpawnParameters.Owner = CharacterActor;

	Command = World->SpawnActor<APF2CharacterCommand>(StaticClass(), SpawnParameters);

	Command->Character         = CharacterActor;
	Command->AbilitySpecHandle = AbilitySpecHandle;

	return Command;
}

void APF2CharacterCommand::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AActor::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APF2CharacterCommand, Character);
	DOREPLIFETIME(APF2CharacterCommand, AbilitySpecHandle);
}

UTexture2D* APF2CharacterCommand::GetCommandIcon() const
{
	return GetAbilityIntf()->GetAbilityIcon();
}

FText APF2CharacterCommand::GetCommandLabel() const
{
	return GetAbilityIntf()->GetAbilityLabel();
}

FText APF2CharacterCommand::GetCommandDescription() const
{
	return GetAbilityIntf()->GetAbilityDescription();
}

EPF2CommandExecuteOrQueueResult APF2CharacterCommand::AttemptExecuteOrQueue()
{
	EPF2CommandExecuteOrQueueResult Result = EPF2CommandExecuteOrQueueResult::None;
	const UWorld* const             World  = this->GetWorld();

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteOrQueue() called on command ('%s')."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs())
	);

	if (World != nullptr)
	{
		IPF2GameModeInterface* PF2GameMode = Cast<IPF2GameModeInterface>(World->GetAuthGameMode());

		if (PF2GameMode != nullptr)
		{
			TScriptInterface<IPF2CharacterInterface> CharacterIntf = this->GetCharacter();

			TScriptInterface<IPF2CharacterCommandInterface> CommandIntf =
				PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(this);

			Result = PF2GameMode->AttemptToExecuteOrQueueCommand(CharacterIntf, CommandIntf);
		}
	}

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteOrQueue() result for command ('%s'): %s."),
		*(PF2LogUtilities::GetHostNetId(World)),
		*(this->GetIdForLogs()),
		*(PF2EnumUtilities::ToString(Result))
	);

	return Result;
}

EPF2CommandExecuteImmediatelyResult APF2CharacterCommand::AttemptExecuteImmediately()
{
	EPF2CommandExecuteImmediatelyResult Result;

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteImmediately() called on command ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs())
	);

	if (this->GetAbilitySystemComponent()->TryActivateAbility(this->GetAbilitySpecHandle()))
	{
		Result = EPF2CommandExecuteImmediatelyResult::Activated;
	}
	else
	{
		Result = EPF2CommandExecuteImmediatelyResult::Blocked;
	}

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] AttemptExecuteImmediately() result for command ('%s'): %s."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetIdForLogs()),
		*(PF2EnumUtilities::ToString(Result))
	);

	return Result;
}

void APF2CharacterCommand::Cancel()
{
	this->GetCharacter()->GetCommandQueueComponent()->Remove(this);
}

FString APF2CharacterCommand::GetIdForLogs() const
{
	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}[{1}.{2}]"),
		{
			*(this->GetCommandLabel().ToString()),
			*(this->GetAbility()->GetName()),
			*(this->GetName())
		}
	);
}
