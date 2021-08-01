﻿// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <CoreMinimal.h>
#include "RPGAbilityCalculationBase.generated.h"

/**
 * Base class for MMCs that provide values based on captured character ability values.
 */
UCLASS(Abstract)
class TEENWOLFRPG_API URPGAbilityCalculationBase : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	/**
	 * Calculates an ability-based modifier value based on the attribute captured by the provided GE specification.
	 *
	 * Most sub-classes should not need to override this implementation. Instead, this implementation does most of the
	 * hard work of extracting the ability score value and then calling DoCalculation(FGameplayAttribute, float).
	 *
	 * @param Spec
	 *   The Gameplay Effect (GE) specification that provides information about the ability score for which a calculated
	 *   value is desired.
	 *
	 * @return
	 *   The calculated value.
	 */
	virtual float DoCalculation(const FGameplayEffectSpec& Spec) const;

	/**
	 * Calculates an ability-based modifier value based on the value and metadata of a captured attribute.
	 *
	 * All sub-classes must override this implementation.
	 *
	 * @param AbilityAttribute
	*   The type of ability score for which a calculated value is desired.
	 * @param AbilityScore
	 *   The current base value of the ability attribute.
	 *
	 * @return
	 *   The calculated value.
	 */
	virtual float DoCalculation(const FGameplayAttribute AbilityAttribute, const float AbilityScore) const
	PURE_VIRTUAL(URPGAbilityCalculationBase::DoCalculation, return 0;);
};
