// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Abilities/PF2CharacterAbilityScoreType.h"
#include "PF2AbilitySystemInterface.h"
#include "PF2AttributeModifierSnapshot.h"

#include "PF2CharacterAbilitySystemInterface.generated.h"

// =====================================================================================================================
// Forward Declarations (to break recursive dependencies)
// =====================================================================================================================
// This is defined in "Abilities/PF2GameplayAbility_BoostAbilityBase.h", but that file depends on this header file, so
// we have to break the recursive dependency.
class UPF2AbilityBoostBase;

// =====================================================================================================================
// Normal Declarations
// =====================================================================================================================
UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UPF2CharacterAbilitySystemInterface : public UPF2AbilitySystemInterface
{
    GENERATED_BODY()
};

/**
 * A more specific OpenPF2 interface for Ability System Components that are owned by characters having ability scores.
 *
 * This interface provides some additional convenience/utility functionality that allows direct manipulation of the
 * ability scores of a character by means of activating passive GEs on the ASC.
 */
class OPENPF2CORE_API IPF2CharacterAbilitySystemInterface : public IPF2AbilitySystemInterface
{
    GENERATED_BODY()

public:
	/**
	 * Gets the level of the owning character.
	 *
	 * This requires the owning actor to implement IPF2CharacterInterface. If the owning actor does not implement that
	 * interface, the default level of 1 is returned instead.
	 *
	 * @return
	 *	The level of the owning character actor.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Ability System")
	virtual int32 GetCharacterLevel() const = 0;

	/**
	 * Gets a snapshot from this ASC of all current character ability scores and their modifiers.
	 *
	 * @return
	 *	A map from character ability scores to a snapshot of their values and modifiers.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Ability System")
	virtual TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> GetAbilityScoreValues() const = 0;

	/**
	 * Gets all of the ability boosts that have been granted on this ASC.
	 *
	 * @return
	 *	The ability boost GAs that are still pending for this character.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Ability System")
	virtual TArray<UPF2AbilityBoostBase *> GetPendingAbilityBoosts() const = 0;

	/**
	 * Gets the Gameplay Effect to use as a passive GE when boosting the specified character ability score.
	 *
	 * @param AbilityScore
	 *	The ability score for which a boost is desired.
	 *
	 * @return
	 *	The blueprint to apply as a passive GE to boost that ability.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Ability System")
	virtual TSubclassOf<UGameplayEffect> GetBoostEffectForAbility(const EPF2CharacterAbilityScoreType AbilityScore) = 0;

	/**
	 * Adds a boost of the specified ability to the attribute set of the owning character.
	 *
	 * This results in a passive GE being added to the ASC of the character. The GE is added to the weight group
	 * specified by a tag on GE; this is known as the "default" weight group of the GE.
	 *
	 * If the group into which the GE is being added is already active, then the GE for the boost is activated
	 * immediately. In addition, any Passive GEs in weight groups after the default weight group of the GE are
	 * automatically re-applied.
	 *
	 *
	 * @param TargetAbilityScore
	 *	The ability score that will be boosted.
	 */
	UFUNCTION(BlueprintCallable, Category="OpenPF2|Components|Character Ability System")
    virtual void ApplyAbilityBoost(const EPF2CharacterAbilityScoreType TargetAbilityScore) = 0;
};
