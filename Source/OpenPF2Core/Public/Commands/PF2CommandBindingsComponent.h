// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <Components/ActorComponent.h>

#include "PF2CommandBindingsInterface.h"
#include "PF2CommandInputBinding.h"

#include "PF2CommandBindingsComponent.generated.h"

/**
 * A component for characters that need to support binding execution of Gameplay Abilities to input actions.
 *
 * Unlike the native input binding support offered by the GAS, this version does not bind input directly to a specific
 * pawn/character. Instead, it allows input to be routed to whichever character the player is currently controlling, and
 * it works automatically with abilities that need to be queued during encounters.
 */
UCLASS(ClassGroup="OpenPF2", meta=(BlueprintSpawnableComponent))
class OPENPF2CORE_API UPF2CommandBindingsComponent : public UActorComponent, public IPF2CommandBindingsInterface
{
	GENERATED_BODY()

	/**
	 * The input component to which this component is currently wired.
	 */
	UPROPERTY()
	UInputComponent* InputComponent;

	/**
	 * The association between inputs and Gameplay Abilities.
	 */
	UPROPERTY()
	TArray<FPF2CommandInputBinding> Bindings;

public:
	/**
	 * Default constructor for UPF2CommandBindingsComponent.
	 */
	explicit UPF2CommandBindingsComponent() : InputComponent(nullptr)
	{
	}

	// =================================================================================================================
	// Public Methods - IPF2CommandBindingsInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual void ClearBindings() override;

	UFUNCTION(BlueprintCallable)
	virtual void LoadAbilitiesFromCharacter(const TScriptInterface<IPF2CharacterInterface> Character) override;

	virtual void LoadAbilitiesFromCharacter(IPF2CharacterInterface* Character) override;

	UFUNCTION(BlueprintCallable)
	virtual void ConnectToInput(UInputComponent* NewInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual void DisconnectFromInput() override;

	UFUNCTION(Server, Reliable)
	virtual void ExecuteBoundAbility(const FGameplayAbilitySpecHandle AbilitySpecHandle,
									 AActor* CharacterActor) override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual FString GetIdForLogs() const override;

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Gets the input component to which this bindings component is wired up to, if it is wired up.
	 *
	 * @return
	 *	- If this component is currently wired-up to input: the input component to which it is wired up.
	 *	- If this component is not currently wired-up to input: null.
	 */
	FORCEINLINE UInputComponent* GetInputComponent() const
	{
		return this->InputComponent;
	}

	/**
	 * Gets whether this component is currently wired up to input.
	 *
	 * @return
	 *	- true if this component is currently wired up to input.
	 *	- false if it is not currently wired up to input.
	 */
	FORCEINLINE bool IsConnectedToInput() const
	{
		return this->GetInputComponent() != nullptr;
	}
};
