﻿// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandInputBinding.h"

#include "Commands/PF2CharacterCommand.h"

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
	if (Binding != nullptr)
	{
		Binding->InvokeAbility();
	}
}

void FPF2CommandInputBinding::LocalInputReleased(FPF2CommandInputBinding* Binding)
{
	if (Binding != nullptr)
	{
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
	UPF2CharacterCommand* Command = UPF2CharacterCommand::Create(this->Character, this->AbilitySpecHandle);

	Command->AttemptExecuteOrQueue();
}

void FPF2CommandInputBinding::ReleaseAbility()
{
	// Default implementation -- Do nothing.
}
