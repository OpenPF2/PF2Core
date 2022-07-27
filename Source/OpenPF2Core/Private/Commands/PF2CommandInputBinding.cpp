// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandInputBinding.h"

#include "OpenPF2Core.h"

#include "Commands/PF2CharacterCommand.h"
#include "Commands/PF2CommandBindingsInterface.h"

void FPF2CommandInputBinding::ConnectToInput(UInputComponent* InputComponent)
{
	if (!this->IsConnectedToInput() && !this->ActionName.IsNone())
	{
		TArray<int32> NewHandles;

		// Pressed event
		NewHandles.Add(
			this->AddActionBinding(
				InputComponent,
				IE_Pressed,
				&FPF2CommandInputBinding::LocalInputPressed
			)
		);

		// Released event
		NewHandles.Add(
			this->AddActionBinding(
				InputComponent,
				IE_Released,
				&FPF2CommandInputBinding::LocalInputReleased
			)
		);

		this->Handles = NewHandles;
	}
}

void FPF2CommandInputBinding::DisconnectFromInput(UInputComponent* InputComponent)
{
	if (this->IsConnectedToInput())
	{
		for (const auto& Handle : this->Handles)
		{
			InputComponent->RemoveActionBindingForHandle(Handle);
		}

		this->Handles.Empty();
	}
}

void FPF2CommandInputBinding::LocalInputPressed(FPF2CommandInputBinding* Binding)
{
	if (Binding == nullptr)
	{
		UE_LOG(
			LogPf2CoreKeyBindings,
			VeryVerbose,
			TEXT("Input PRESSED for a null binding.")
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreKeyBindings,
			VeryVerbose,
			TEXT("Input PRESSED for binding of action '%s' for character '%s'."),
			*(Binding->ActionName.ToString()),
			*(Binding->Character->GetIdForLogs())
		);

		Binding->InvokeAbility();
	}
}

void FPF2CommandInputBinding::LocalInputReleased(FPF2CommandInputBinding* Binding)
{
	if (Binding == nullptr)
	{
		UE_LOG(
			LogPf2CoreKeyBindings,
			VeryVerbose,
			TEXT("Input RELEASED for a null binding.")
		);
	}
	else
	{
		UE_LOG(
			LogPf2CoreKeyBindings,
			VeryVerbose,
			TEXT("Input RELEASED for binding to action '%s' for character '%s'."),
			*(Binding->ActionName.ToString()),
			*(Binding->Character->GetIdForLogs())
		);

		Binding->ReleaseAbility();
	}
}

int32 FPF2CommandInputBinding::AddActionBinding(UInputComponent*  InputComponent,
                                                const EInputEvent InKeyEvent,
                                                void              (*Callback)(FPF2CommandInputBinding*))
{
	FInputActionBinding ActionBinding = FInputActionBinding(this->ActionName, InKeyEvent),
	                    AddResult;

	ActionBinding.ActionDelegate.GetDelegateForManualSet().BindStatic(
		Callback,
		this
	);

	AddResult = InputComponent->AddActionBinding(ActionBinding);

	return AddResult.GetHandle();
}

void FPF2CommandInputBinding::InvokeAbility()
{
	// We have to convert the character to an actor for replication to work properly through the RPC.
	this->BindingsOwner->ExecuteBoundAbility(
		this->AbilitySpecHandle,
		this->Character->ToActor()
	);
}

void FPF2CommandInputBinding::ReleaseAbility()
{
	// Default implementation -- Do nothing.
}
