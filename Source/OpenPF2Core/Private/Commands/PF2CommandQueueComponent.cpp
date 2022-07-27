// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandQueueComponent.h"
#include "Commands/PF2CharacterCommandInterface.h"

#include "Utilities/PF2InterfaceUtilities.h"

UPF2CommandQueueComponent::UPF2CommandQueueComponent()
{
}

void UPF2CommandQueueComponent::Enqueue(const TScriptInterface<IPF2CharacterCommandInterface> Command)
{
	IPF2CharacterCommandInterface* CommandIntf = PF2InterfaceUtilities::FromScriptInterface(Command);

	checkf(!this->Queue.Contains(CommandIntf), TEXT("The same command can only exist in the queue once."));
	this->Queue.Add(CommandIntf);

	this->OnCommandAdded.Broadcast(Command);
	this->OnCommandsChanged.Broadcast(PF2InterfaceUtilities::ToScriptInterfaces(this->Queue));
}

void UPF2CommandQueueComponent::PeekNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	if (this->Count() != 0)
	{
		IPF2CharacterCommandInterface* NextCommandPtr = this->Queue[0];

		NextCommand = PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(NextCommandPtr);
	}
}

void UPF2CommandQueueComponent::PopNext(TScriptInterface<IPF2CharacterCommandInterface>& NextCommand)
{
	if (this->Count() != 0)
	{
		IPF2CharacterCommandInterface* NextCommandIntf = this->Queue.Pop();

		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Popping command ('%s') from command queue ('%s')."),
			*(NextCommandIntf->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		NextCommand = PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(NextCommandIntf);

		this->OnCommandRemoved.Broadcast(NextCommand);
		this->OnCommandsChanged.Broadcast(PF2InterfaceUtilities::ToScriptInterfaces(this->Queue));
	}
}

void UPF2CommandQueueComponent::DropNext()
{
	if (this->Count() != 0)
	{
		IPF2CharacterCommandInterface* NextCommandIntf = this->Queue.Pop();

		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing command ('%s') from command queue ('%s')."),
			*(NextCommandIntf->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		this->OnCommandRemoved.Broadcast(
			PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterCommandInterface>(NextCommandIntf)
		);

		this->OnCommandsChanged.Broadcast(PF2InterfaceUtilities::ToScriptInterfaces(this->Queue));
	}
}

EPF2CommandExecuteImmediatelyResult UPF2CommandQueueComponent::PopAndExecuteNext()
{
	EPF2CommandExecuteImmediatelyResult             Result;
	TScriptInterface<IPF2CharacterCommandInterface> NextCommand;

	// We don't pop the command (yet) because it may be blocked and we don't want it to lose its place in the queue if
	// it is.
	this->PeekNext(NextCommand);

	if (NextCommand == nullptr)
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("No actions are currently queued for command queue ('%s')."),
			*(this->GetIdForLogs())
		);

		Result = EPF2CommandExecuteImmediatelyResult::None;
	}
	else
	{
		IPF2CharacterCommandInterface* NextCommandIntf = PF2InterfaceUtilities::FromScriptInterface(NextCommand);

		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Attempt to execute next command ('%s') in command queue ('%s')."),
			*(NextCommandIntf->GetIdForLogs()),
			*(this->GetIdForLogs())
		);

		Result = NextCommandIntf->AttemptExecuteImmediately();

		if (Result == EPF2CommandExecuteImmediatelyResult::Blocked)
		{
			UE_LOG(
				LogPf2Core,
				VeryVerbose,
				TEXT("Next command ('%s') in command queue ('%s') was blocked and will not be removed from the queue."),
				*(NextCommandIntf->GetIdForLogs()),
				*(this->GetIdForLogs())
			);
		}
		else
		{
			// Now it's safe to drop the command.
			this->DropNext();
		}
	}

	return Result;
}

bool UPF2CommandQueueComponent::Remove(const TScriptInterface<IPF2CharacterCommandInterface> Command)
{
	IPF2CharacterCommandInterface* CommandIntf        = PF2InterfaceUtilities::FromScriptInterface(Command);
	const int32                    CountOfRemoved     = this->Queue.Remove(CommandIntf);
	const bool                     bWasCommandRemoved = (CountOfRemoved > 0);

	if (bWasCommandRemoved)
	{
		this->OnCommandRemoved.Broadcast(Command);
		this->OnCommandsChanged.Broadcast(PF2InterfaceUtilities::ToScriptInterfaces(this->Queue));
	}

	return bWasCommandRemoved;
}

int UPF2CommandQueueComponent::Count()
{
	return this->Queue.Num();
}

void UPF2CommandQueueComponent::Clear()
{
	this->Queue.Empty();
	this->OnCommandsChanged.Broadcast(TArray<TScriptInterface<IPF2CharacterCommandInterface>>());
}

FString UPF2CommandQueueComponent::GetIdForLogs() const
{
	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}.{1}"),
		{
			*(this->GetOwner()->GetName()),
			*(this->GetName())
		}
	);
}
