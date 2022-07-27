// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayAbilitySpec.h>

#include <Components/InputComponent.h>

#include "PF2CommandInputBinding.generated.h"

// =====================================================================================================================
// Forward Declarations (to minimize header dependencies)
// =====================================================================================================================
class IPF2CharacterInterface;
class IPF2CommandBindingsInterface;
class UGameplayAbility;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
/**
 * A single binding between an input action and an ability that can be activated.
 *
 * The ability must already have been granted to the character.
 */
USTRUCT(BlueprintType)
struct FPF2CommandInputBinding
{
	GENERATED_BODY()

	// =================================================================================================================
	// Public Properties - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The human-friendly name of the action, as configured in project input settings (e.g "Jump", "Fire", etc.).
	 */
	UPROPERTY(EditDefaultsOnly)
	FName ActionName;

protected:
	// =================================================================================================================
	// Protected Properties - Blueprint Accessible
	// =================================================================================================================
	/**
	 * The handle for the corresponding ability.
	 */
	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	/**
	 * The handles for input action bindings, if this binding been connected to input.
	 */
	TArray<int32> Handles;

	// =================================================================================================================
	// Protected Properties
	// =================================================================================================================
	/**
	 * The character to which the ability has been granted.
	 */
	IPF2CharacterInterface* Character;

	/**
	 * The component that is managing this binding.
	 */
	IPF2CommandBindingsInterface* BindingsOwner;

public:
	// =================================================================================================================
	// Public Constructors
	// =================================================================================================================
	/**
	 * Default constructor (used by Blueprint).
	 */
	explicit FPF2CommandInputBinding() : Character(nullptr), BindingsOwner(nullptr)
	{
	}

	/**
	 * Constructor for initializing a FPF2CommandInputBinding from a granted ability.
	 *
	 * @param ActionName
	 *	The human-friendly name of the action, as configured in project settings (e.g "Jump").
	 * @param AbilitySpec
	 *	The specification for the ability.
	 * @param Character
	 *	The character to which the ability has been granted.
	 */
	explicit FPF2CommandInputBinding(const FName&                  ActionName,
	                                 const FGameplayAbilitySpec    AbilitySpec,
	                                 IPF2CharacterInterface*       Character,
	                                 IPF2CommandBindingsInterface* Owner) :
		ActionName(ActionName),
		AbilitySpecHandle(AbilitySpec.Handle),
		Character(Character),
		BindingsOwner(Owner)
	{
	}

	// =================================================================================================================
	// Public Destructors
	// =================================================================================================================
	/**
	 * Default destructor.
	 */
	virtual ~FPF2CommandInputBinding() = default;

	// =================================================================================================================
	// Public Methods
	// =================================================================================================================
	/**
	 * Determines whether the ability in this binding has been connected to an input component.
	 *
	 * @return
	 *	- true if this binding is presently connected to input.
	 *	- false if this binding is not connected to input at this time.
	 */
	FORCEINLINE bool IsConnectedToInput() const
	{
		return this->Handles.Num() != 0;
	}

	/**
	 * Wires-up this binding to receive input from the given player input component.
	 *
	 * This only has an effect if this binding has an action assigned and this binding has not yet been wired up;
	 * otherwise, this call will have no effect.
	 *
	 * @param InputComponent
	 *	The component to which input should be bound.
	 */
	void ConnectToInput(UInputComponent* InputComponent);

	/**
	 * Removes this binding from the given player input component.
	 *
	 * This must be called before changing the action of any binding.
	 *
	 * This only has an effect if this binding has been wired up; otherwise, this call will have no effect.
	 */
	void DisconnectFromInput(UInputComponent* InputComponent);

protected:
	// =================================================================================================================
	// Protected Static Methods
	// =================================================================================================================
	/**
	 * Notifies the specified binding instance that the input action it corresponds to has been activated.
	 *
	 * @param Binding
	 *	The binding to notify of a button activation.
	 */
	static void LocalInputPressed(FPF2CommandInputBinding* Binding);

	/**
	* Notifies the specified binding instance that the input action it corresponds to has been released.
	 *
	 * @param Binding
	 *	The binding to notify of a button release.
	 */
	static void LocalInputReleased(FPF2CommandInputBinding* Binding);

	// =================================================================================================================
	// Protected Instance Methods
	// =================================================================================================================
	int32 AddActionBinding(UInputComponent*  InputComponent,
	                       const EInputEvent InKeyEvent,
	                       void              (*Callback)(FPF2CommandInputBinding*));

	/**
	 * Assembles and executes a command that invokes the ability associated with this binding.
	 *
	 * The ability may be queued if the active MoPRS is requiring abilities to be queued (e.g., encounters).
	 */
	virtual void InvokeAbility();

	/**
	 * Notifies the ability that the player is no longer pressing the keys for this binding.
	 *
	 * (The default implementation does nothing, so this call has no effect).
	 */
	virtual void ReleaseAbility();
};
