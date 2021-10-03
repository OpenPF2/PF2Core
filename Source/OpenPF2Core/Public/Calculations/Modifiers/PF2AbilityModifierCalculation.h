﻿// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
//
// Content from Pathfinder 2nd Edition is licensed under the Open Game License (OGL) v1.0a, subject to the following:
//   - Open Game License v 1.0a, Copyright 2000, Wizards of the Coast, Inc.
//   - System Reference Document, Copyright 2000, Wizards of the Coast, Inc.
//   - Pathfinder Core Rulebook (Second Edition), Copyright 2019, Paizo Inc.
// Except for material designated as Product Identity, the game mechanics and logic in this file are Open Game Content,
// as defined in the Open Game License version 1.0a, Section 1(d) (see accompanying LICENSE.TXT). No portion of this
// file other than the material designated as Open Game Content may be reproduced in any form without written
// permission.

#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <CoreMinimal.h>

#include "Calculations/PF2AbilityCalculationBase.h"
#include "PF2AbilityModifierCalculation.generated.h"

/**
 * Base MMC used for determining ability modifiers from normal ability scores.
 *
 * There is expected to be one blueprint sub-class of this base class for each ability (Strength, Dexterity, Constitution,
 * etc). The ability for which the modifier is being calculated *must* be the one and ONLY attribute being captured.
 */
UCLASS(Abstract)
class OPENPF2CORE_API UPF2AbilityModifierCalculation : public UPF2AbilityCalculationBase
{
	GENERATED_BODY()

protected:
	// =================================================================================================================
	// Protected Methods
	// =================================================================================================================
	/**
	 * Calculates an ability modifier based on the attribute captured by the provided GE specification.
	 *
	 * According to "Table 1-1: Ability Modifiers" in the Pathfinder 2E Core Rulebook, the ability modifier for an
	 * ability is equal to:
	 *
	 * Floor(Score / 2) - 5
	 *
	 * So:
	 *  1 => -5
	 *  2 => -4
	 *  3 => -4
	 * 10 =>  0
	 * 11 =>  0
	 * 24 => +7
	 * 25 => +7
	 *
	 * ... and so on.
	 *
	 * @param Spec
	 *	The Gameplay Effect (GE) specification that provides information about the ability score for which a calculated
	 *	value is desired.
	 * @param AbilityAttribute
	 *	The type of ability score for which an ability modifier is desired.
	 * @param AbilityScore
	 *	The current base value of the ability attribute.
	 *
	 * @return
	 *	The ability modifier.
	 */
	virtual float DoCalculation(const FGameplayEffectSpec& Spec,
	                            const FGameplayAttribute   AbilityAttribute,
	                            const float                AbilityScore) const override;
};