// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Commands/PF2CommandBindingsComponent.h"

#include <AbilitySystemComponent.h>
#include <Components/InputComponent.h>

#include "Abilities/PF2GameplayAbilityInterface.h"

#include "PF2CharacterInterface.h"

#include "Commands/PF2CharacterCommand.h"
#include "Commands/PF2CharacterCommandInterface.h"
#include "Commands/PF2CommandInputBinding.h"

#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

void UPF2CommandBindingsComponent::ClearBindings()
{
	if (this->IsConnectedToInput())
	{
		for (FPF2CommandInputBinding& Binding : this->Bindings)
		{
			Binding.DisconnectFromInput(this->GetInputComponent());
		}
	}

	this->Bindings.Empty();
}

void UPF2CommandBindingsComponent::LoadAbilitiesFromCharacter(const TScriptInterface<IPF2CharacterInterface> Character)
{
	this->LoadAbilitiesFromCharacter(PF2InterfaceUtilities::FromScriptInterface(Character));
}

void UPF2CommandBindingsComponent::LoadAbilitiesFromCharacter(IPF2CharacterInterface* Character)
{
	UAbilitySystemComponent*     AbilitySystemComponent = Character->GetAbilitySystemComponent();
	TArray<FGameplayAbilitySpec> ActivatableAbilities   = AbilitySystemComponent->GetActivatableAbilities();
	int32                        NumMappedAbilities     = 0;

	checkf(
		this->Bindings.Num() == 0,
		TEXT("Abilities must be loaded from a character before custom bindings are added.")
	);

	UE_LOG(
		LogPf2CoreKeyBindings,
		VeryVerbose,
		TEXT("[%s] Loading %d abilities from Character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		ActivatableAbilities.Num(),
		*(Character->GetIdForLogs())
	);

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities)
	{
		const UGameplayAbility*             Ability       = AbilitySpec.Ability;
		const IPF2GameplayAbilityInterface* AbilityIntf   = Cast<IPF2GameplayAbilityInterface>(Ability);
		FName                               DefaultAction;

		if (AbilityIntf != nullptr)
		{
			DefaultAction = AbilityIntf->GetDefaultInputActionMapping();
			++NumMappedAbilities;
		}
		else
		{
			// Fallback for interoperability with non-PF2 abilities.
			DefaultAction = FName();
		}

		this->Bindings.Add(FPF2CommandInputBinding(DefaultAction, AbilitySpec, Character, this));
	}

	UE_LOG(
		LogPf2CoreKeyBindings,
		VeryVerbose,
		TEXT("[%s] Loaded %d abilities with default action mappings from Character ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		NumMappedAbilities,
		*(Character->GetIdForLogs())
	);

	if (this->IsConnectedToInput())
	{
		// Wire up all the new bindings.
		this->ConnectToInput(this->GetInputComponent());
	}
}

void UPF2CommandBindingsComponent::ConnectToInput(UInputComponent* NewInputComponent)
{
	checkf(
		!this->IsConnectedToInput() || (this->InputComponent == NewInputComponent),
		TEXT("Command bindings cannot be wired-up to two different input components at the same time.")
	);

	for (FPF2CommandInputBinding& Binding : this->Bindings)
	{
		Binding.ConnectToInput(NewInputComponent);
	}

	this->InputComponent = NewInputComponent;
}

void UPF2CommandBindingsComponent::DisconnectFromInput()
{
	if (this->IsConnectedToInput())
	{
		for (FPF2CommandInputBinding& Binding : this->Bindings)
		{
			Binding.DisconnectFromInput(this->InputComponent);
		}

		this->InputComponent = nullptr;
	}
}

void UPF2CommandBindingsComponent::ExecuteBoundAbility_Implementation(
	const FGameplayAbilitySpecHandle AbilitySpecHandle,
	AActor*                          CharacterActor)
{
	UE_LOG(
		LogPf2CoreKeyBindings,
		VeryVerbose,
		TEXT("[%s] ExecuteBoundAbility_Implementation() called."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld()))
	);

	IPF2CharacterCommandInterface* Command = APF2CharacterCommand::Create(CharacterActor, AbilitySpecHandle);
	Command->AttemptExecuteOrQueue();
}

FString UPF2CommandBindingsComponent::GetIdForLogs() const
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
