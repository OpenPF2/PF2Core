// OpenPF2 for UE Game Logic, Copyright 2021, Guy Elsmore-Paddock. All Rights Reserved.
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

#include "Abilities/GameplayAbility.h"
#include "Abilities/PF2CharacterAttributeStatics.h"
#include "Abilities/PF2CharacterAbilityScoreType.h"
#include "Abilities/Weapons/PF2WeaponInterface.h"
#include "PF2MonetaryValue.h"

#include "PF2WeaponBase.generated.h"

/**
 * Enumerated type for the number of hands a character has to have free in order to hold a weapon.
 */
UENUM(BlueprintType)
enum class EPF2WeaponHandsRequirement : uint8
{
	One     UMETA(DisplayName="1  - One"),
	OnePlus UMETA(DisplayName="1+ - One to hold; two to fire"),
	Two     UMETA(DisplayName="2  - Two"),
};

/**
 * Base class for weapons a character can equip.
 *
 * This is the top-most C++ base class for weapons, but there are several Blueprint sub-classes of this class that offer
 * functionality that tends to be a more complete fit for your own weapon implementations.
 */
UCLASS(Abstract)
// ReSharper disable once CppClassCanBeFinal
class OPENPF2CORE_API UPF2WeaponBase : public UGameplayAbility, public IPF2WeaponInterface
{
	GENERATED_BODY()

protected:
	/**
	 * The human-friendly name for this weapon.
	 */
	UPROPERTY(EditAnywhere, Category="Weapon Statistics")
	FText Name;

	/**
	 * The root/parent tag of each set of tags that represent a character's TEML proficiencies with this weapon.
	 *
	 * Most weapons will typically define only one root tag here. However, ancestry-specific weapons that are affected
	 * by a weapon familiarity feat will define at least two. The highest-granted proficiency wins.
	 *
	 * For example, a gnome martial weapon would have root gameplay tags of both "WeaponProficiency.Category.Martial"
	 * and "WeaponProficiency.Category.MartialGnome". This would allow a character to have proficiency with the weapon
	 * under the following scenarios:
	 *	- A Gnome with the "Gnome Weapon Familiarity" feat who is proficient with simple weapons, granting them both a
	 *	  "WeaponProficiency.Category.Simple.*" and "WeaponProficiency.Category.MartialGnome.*" gameplay tag, since the
	 *	  feat allows gnome martial weapons to be treated as simple weapons.
	 *	- Any character without the "Gnome Weapon Familiarity" feat who is proficient with martial weapons, since the
	 *	  proficiency grants them a "WeaponProficiency.Category.Martial.*" gameplay tag.
	 */
	UPROPERTY(EditDefaultsOnly, meta=(Categories="WeaponProficiency"), Category="Weapon Statistics")
	FGameplayTagContainer ProficiencyTagPrefixes;

	/**
	 * The type of ability modifier from the character's ASC that is added to *attack* rolls with this weapon.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Weapon Statistics")
	EPF2CharacterAbilityScoreType AttackAbilityModifierType;

	/**
	 * The type of ability modifier from the character's ASC that is added to *damage* rolls with this weapon.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Weapon Statistics")
	EPF2CharacterAbilityScoreType DamageAbilityModifierType;

	/**
	 * The value or price of this item.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Weapon Statistics")
	FPF2MonetaryValue Value;

	/**
	 * A roll expression for the die/dice used to calculate damage from a hit with this weapon.
	 *
	 * For example, "1d4" or "1d6".
	 */
	UPROPERTY(EditDefaultsOnly, Category="Weapon Statistics")
	FName DamageDie;

	/**
	 * The type of damage this weapon does.
	 */
	UPROPERTY(EditDefaultsOnly, meta=(Categories="DamageType"), Category="Weapon Statistics")
	FGameplayTag DamageType;

	/**
	 * How large or bulky this weapon is to carry.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 629, "Bulk":
	 * "[Bulk is a] value indicating an item's size, weight, and general awkwardness. It takes 10 items of light Bulk to
	 * equal 1 Bulk [...] A character becomes encumbered if they carry Bulk greater than 5 + their Str modifier, and
	 * they can't carry more than 10 + their Str modifier."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 271-272, "Bulk":
	 * "Carrying especially heavy or unwieldy items can make it more difficult for you to move, as can overloading
	 * yourself with too much gear. The Bulk value of an item reflects how difficult the item is to handle,
	 * representing its size, weight, and general awkwardness. If you have a high Strength score, you usually don't need
	 * to worry about Bulk unless you're carrying numerous substantial items."
	 *
	 * From the Pathfinder 2E Core Rulebook, page 272, "Bulk Values":
	 * "Items can have a number to indicate their Bulk value, or they can be light (indicated by an L) or negligible
	 * (indicated by a —) for the purpose of determining Bulk. For instance, full plate armor is 4 Bulk, a longsword is
	 * 1 Bulk, a dagger or scroll is light, and a piece of chalk is negligible. Ten light items count as 1 Bulk, and you
	 * round down fractions (so 9 light items count as 0 Bulk, and 11 light items count as 1 Bulk). Items of negligible
	 * Bulk don't count toward Bulk unless you try to carry vast numbers of them, as determined by the GM."
	 *
	 * In OpenPF2, the following Bulk values correspond to special values from the Pathfinder 2E Core Rulebook:
	 *	- "0.01" corresponds to "negligible" (—) bulk.
	 *	- "0.10" corresponds to "light" (L) bulk.
	 */
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=0), Category="Weapon Statistics")
	float Bulk;

	/**
	 * How many hands a character has to have free in order to hold this weapon.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 279:
	 * "Some weapons require one hand to wield, and others require two. A few items, such as a longbow, list 1+ for its
	 * Hands entry. You can hold a weapon with a 1+ entry in one hand, but the process of shooting it requires using a
	 * second to retrieve, nock, and loose an arrow. This means you can do things with your free hand while holding the
	 * bow without changing your grip, but the other hand must be free when you shoot. To properly wield a 1+ weapon,
	 * you must hold it in one hand and also have a hand free.
	 *
	 * Weapons requiring two hands typically deal more damage. Some one-handed weapons have the two-hand trait, causing
	 * them to deal a different size of weapon damage die when used in two hands. In addition, some abilities require
	 * you to wield a weapon in two hands. You meet this requirement while holding the weapon in two hands, even if it
	 * doesn't require two hands or have the two-hand trait."
	 */
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1), Category="Weapon Statistics")
	EPF2WeaponHandsRequirement Hands;

	/**
	 * The group into which this weapon is classified.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 280:
	 * "A weapon or unarmed attack's group classifies it with similar weapons. Groups affect some abilities and what the
	 * weapon does on a critical hit if you have access to that weapon or unarmed attack's critical specialization
	 * effects..."
	 */
	UPROPERTY(EditDefaultsOnly, meta=(Categories="WeaponGroup"), Category="Weapon Statistics")
	FGameplayTag Group;

	/**
	 * The traits of this weapon or unarmed attack.
	 *
	 * From the Pathfinder 2E Core Rulebook, page 637:
	 * "[Traits are keywords] that [convey] information about a rules element. Often a trait indicates how other rules
	 * interact with an ability, creature, item, or other rules element with that trait."
	 */
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Trait.Weapon"), Category="Weapon Statistics")
	FGameplayTagContainer Traits;

public:
	UPF2WeaponBase(): DamageDie("1d6")
	{
	}

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle     Handle,
		const FGameplayAbilityActorInfo*     ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData*            TriggerEventData) override;

protected:
	/**
	 * Calculates the attack roll, which determines if an attack was successful (it hit its target).
	 *
	 * "When making an attack roll, determine the result by rolling 1d20 and adding your attack modifier for the weapon
	 * or unarmed attack you’re using. Modifiers for melee and ranged attacks are calculated differently.
	 *
	 * Melee attack modifier = Strength modifier (or optionally Dexterity for a finesse weapon) + proficiency bonus +
	 * other bonuses + penalties
	 *
	 * Ranged attack modifier = Dexterity modifier + proficiency bonus + other bonuses + penalties
	 *
	 * Bonuses, and penalties apply to these rolls just like with other types of checks. Weapons with potency runes add
	 * an item bonus to your attack rolls."
	 *
	 * Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Attack Rolls".
	 */
	virtual float CalculateAttackRoll(const IPF2CharacterAbilitySystemInterface* CharacterAsc);

	/**
	 * Calculates the damage roll, which determines how much of an effect an attack has on the target.
	 *
	 * "When the result of your attack roll with a weapon or unarmed attack equals or exceeds your target’s AC, you hit
	 * your target! Roll the weapon or unarmed attack’s damage die and add the relevant modifiers, bonuses, and
	 * penalties to determine the amount of damage you deal. Calculate a damage roll as follows.
	 *
	 * Melee damage roll = damage die of weapon or unarmed attack + Strength modifier + bonuses + penalties
	 *
	 * Ranged damage roll = damage die of weapon + Strength modifier for thrown weapons + bonuses + penalties"
	 *
	 * Source: Pathfinder 2E Core Rulebook, Chapter 6, page 278, "Damage Rolls".
	 */
	virtual float CalculateDamageRoll(const IPF2CharacterAbilitySystemInterface* CharacterAsc);

	/**
	 * Gets the value of the specified ability.
	 *
	 * TODO: Re-work this to be a GE so we don't have to worry about snapshot timing and other GEs.
	 */
	static float GetAbilityModifierValue(const IPF2CharacterAbilitySystemInterface* CharacterAsc,
	                                     const EPF2CharacterAbilityScoreType        AbilityScoreType);
};
