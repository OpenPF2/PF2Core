// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/Info.h>

#include <UObject/Object.h>

#include "GameplayAbilitySpec.h"
#include "PF2CharacterCommandInterface.h"
#include "PF2CharacterInterface.h"

#include "Abilities/PF2GameplayAbilityInterface.h"

#include "Commands/PF2CommandExecuteImmediatelyResult.h"
#include "Commands/PF2CommandExecuteOrQueueResult.h"

#include "Utilities/PF2InterfaceUtilities.h"

#include "PF2CharacterCommand.generated.h"

/**
 * A command that wraps a character ability that can be executed at any time in the future (including immediately).
 *
 * (The inspiration for this approach -- of wrapping Gameplay Abilities in a command that can be invoked at any time --
 * came from "ue4-orders-abilities" by Daedalic Entertainment,
 * https://github.com/DaedalicEntertainment/ue4-orders-abilities).
 */
UCLASS(BlueprintType)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API APF2CharacterCommand : public AInfo, public IPF2CharacterCommandInterface
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Properties
	// =================================================================================================================
	/**
	 * The character who would be issued this command.
	 */
	UPROPERTY(Replicated)
	TScriptInterface<IPF2CharacterInterface> Character;

	/**
	 * The handle of the ability that this command will trigger when it is executed.
	 */
	UPROPERTY(Replicated)
	FGameplayAbilitySpecHandle AbilitySpecHandle;

public:
	// =================================================================================================================
	// Public Static Methods
	// =================================================================================================================
	/**
	 * Creates a new APF2CharacterCommand for the given character and ability specification.
	 *
	 * @param InCharacter
	 *	The character who would be issued the command.
	 * @param InAbilitySpecHandle
	 *	The handle of the ability that the command will trigger when it is executed.
	 *
	 * @return
	 *	The new command.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Character Commands")
	static TScriptInterface<IPF2CharacterCommandInterface> Create(
		const TScriptInterface<IPF2CharacterInterface> InCharacter,
		const FGameplayAbilitySpecHandle               InAbilitySpecHandle)
	{
		return PF2InterfaceUtilities::ToScriptInterface(Create(InCharacter->ToActor(), InAbilitySpecHandle));
	}

	/**
	 * Creates a new APF2CharacterCommand for the given character and ability specification.
	 *
	 * @param Character
	 *	The character who would be issued the command.
	 * @param AbilitySpecHandle
	 *	The handle of the ability that the command will trigger when it is executed.
	 *
	 * @return
	 *	The new command.
	 */
	FORCEINLINE static IPF2CharacterCommandInterface* Create(IPF2CharacterInterface*          Character,
	                                                         const FGameplayAbilitySpecHandle AbilitySpecHandle)
	{
		return Create(Character->ToActor(), AbilitySpecHandle);
	}

	/**
	 * Creates a new APF2CharacterCommand for the given character actor and ability specification.
	 *
	 * The given actor must implement IPF2CharacterInterface.
	 *
	 * @param CharacterActor
	 *	The character (as an actor) who would be issued the command.
	 * @param AbilitySpecHandle
	 *	The handle of the ability that the command will trigger when it is executed.
	 *
	 * @return
	 *	The new command.
	 */
	static IPF2CharacterCommandInterface* Create(
		AActor*                          CharacterActor,
		const FGameplayAbilitySpecHandle AbilitySpecHandle);

protected:
	// =================================================================================================================
	// Protected Constructors
	// =================================================================================================================
	explicit APF2CharacterCommand() : Character(nullptr)
	{
	}

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// =================================================================================================================
	// Public Methods - IPF2CharacterCommandInterface Overrides
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual UTexture2D* GetCommandIcon() const override;

	UFUNCTION(BlueprintCallable)
	virtual FText GetCommandLabel() const override;

	UFUNCTION(BlueprintCallable)
	virtual FText GetCommandDescription() const override;

	UFUNCTION(BlueprintCallable)
	virtual EPF2CommandExecuteOrQueueResult AttemptExecuteOrQueue() override;

	UFUNCTION(BlueprintCallable)
	virtual EPF2CommandExecuteImmediatelyResult AttemptExecuteImmediately() override;

	UFUNCTION(BlueprintCallable)
	virtual void Cancel() override;

	// =================================================================================================================
	// Public Methods - IPF2LogIdentifiableInterface Implementation
	// =================================================================================================================
	UFUNCTION(BlueprintCallable)
	virtual FString GetIdForLogs() const override;

	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
protected:
	/**
	 * Gets the character who would be issued this command.
	 *
	 * @return
	 *	The character.
	 */
	TScriptInterface<IPF2CharacterInterface> GetCharacter() const
	{
		check(this->Character);
		return this->Character;
	}

	/**
	 * Gets the handle of the ability that this command will trigger when it is executed.
	 *
	 * @return
	 *	The ability handle.
	 */
	FORCEINLINE FGameplayAbilitySpecHandle GetAbilitySpecHandle() const
	{
		return this->AbilitySpecHandle;
	}

	/**
	 * Gets the specification for the ability that this command will trigger when it is executed.
	 *
	 * @return
	 *	The ability specification.
	 */
	FORCEINLINE FGameplayAbilitySpec* GetAbilitySpec() const
	{
		return this->GetAbilitySystemComponent()->FindAbilitySpecFromHandle(this->GetAbilitySpecHandle());
	}

	/**
	 * Gets the Ability System Component (ASC) of the character for which this command will be executed.
	 */
	UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		UAbilitySystemComponent* AbilitySystemComponent = this->GetCharacter()->GetAbilitySystemComponent();
		check(AbilitySystemComponent);
		return AbilitySystemComponent;
	}

	/**
	 * Gets the CDO for the ability that this command will trigger when it is executed.
	 *
	 * @return
	 *	The gameplay ability.
	 */
	FORCEINLINE UGameplayAbility* GetAbility() const
	{
		UGameplayAbility* Ability = this->GetAbilitySpec()->Ability;
		check(Ability);
		return Ability;
	}

	/**
	 * Gets the PF2 interface to the CDO of the ability that this command will trigger when it is executed.
	 *
	 * @return
	 *	The gameplay ability, as a PF2 interface.
	 */
	FORCEINLINE IPF2GameplayAbilityInterface* GetAbilityIntf() const
	{
		IPF2GameplayAbilityInterface* Ability = Cast<IPF2GameplayAbilityInterface>(this->GetAbility());
		check(Ability);
		return Ability;
	}
};
