// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
//
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include "PF2AbilityBoostRuleOption.h"
#include "PF2AttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "Utilities/PF2GameplayAbilityUtilities.h"

#include "PF2AbilityBoostBase.generated.h"

/**
 * Base class for a Gameplay Ability that gives the player the option to boost one or more character abilities.
 *
 * Each sub-class/instance of this class represents ability boosts that are coming from a single source (ancestry,
 * background, class, etc.) at a specific point in time. As such, when this ability is invoked by the player, care must
 * be taken to ensure that the same ability is not boosted more than once per activation.
 *
 * From the Pathfinder 2E Core Rulebook, page 20, "Ability Boosts":
 * "When your character receives an ability boost, the rules indicate whether it must be applied to a specific
 * ability score or to one of two specific ability scores, or whether it is a 'free' ability boost that can be
 * applied to any ability score of your choice. However, when you gain multiple ability boosts at the same time, you
 * must apply each one to a different score."
 */
UCLASS(Abstract, HideCategories=("Triggers"))
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2AbilityBoostBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/**
	 * The description of this boost, as shown to the player when they are being asked to make a selection.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Description")
	FText Description;

	/**
	 * The boost(s) this ability applies to the target.
	 *
	 * Each element represents a single rule option for an ability score boost. Each can either be a free ability boost;
	 * or the option to choose between one or more ability options.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability Boost Options")
	TArray<FPF2AbilityBoostRuleOption> BoostRuleOptions;

public:
	UPF2AbilityBoostBase();

	FORCEINLINE static FGameplayTag GetTriggerTag()
	{
		return PF2GameplayAbilityUtilities::GetTag(FName("GameplayAbility.Type.AbilityBoost"));
	};

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer*           OptionalRelevantTags) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle     Handle,
		const FGameplayAbilityActorInfo*     ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData*            TriggerEventData) override;

protected:
	/**
	 * Gets which boosts were selected and passed-in to a GA activation.
	 *
	 * @return
	 *	The choice(s) for ability boosts. The value of each element is the chosen ability and the key is the rule option
	 *	to which the choice corresponds.
	 */
	TSet<EPF2CharacterAbilityScoreType> GetBoostSelections(const FGameplayEventData* TriggerEventData) const;
};
