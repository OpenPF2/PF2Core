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

#include "Abilities/PF2AttributeSet.h"

#include <GameplayEffectExtension.h>
#include <GameFramework/Controller.h>
#include <Net/UnrealNetwork.h>

#include "AbilitySystemBlueprintLibrary.h"
#include "PF2CharacterInterface.h"

UPF2AttributeSet::UPF2AttributeSet() :
	Experience(0.0f),
	AbBoostCount(0.0f),
	AbBoostLimit(0.0f),
	AbStrength(10.0f),
	AbStrengthModifier(0.0f),
	AbDexterity(10.0f),
	AbDexterityModifier(0.0f),
	AbConstitution(10.0f),
	AbConstitutionModifier(0.0f),
	AbIntelligence(10.0f),
	AbIntelligenceModifier(0.0f),
	AbWisdom(10.0f),
	AbWisdomModifier(0.0f),
	AbCharisma(10.0f),
	AbCharismaModifier(0.0f),
	ClassDifficultyClass(0.0f),
	Speed(1.0f),
	MaxSpeed(1.0f),
	ArmorClass(10.0f),
	StFortitudeModifier(0.0f),
	StReflexModifier(0.0f),
	StWillModifier(0.0f),
	HitPoints(1.0f),
	MaxHitPoints(1.0f),
	RstPhysicalBludgeoning(0.0f),
	RstPhysicalPiercing(0.0f),
	RstPhysicalSlashing(0.0f),
	RstEnergyAcid(0.0f),
	RstEnergyCold(0.0f),
	RstEnergyFire(0.0f),
	RstEnergySonic(0.0f),
	RstEnergyPositive(0.0f),
	RstEnergyNegative(0.0f),
	RstEnergyForce(0.0f),
	RstAlignmentChaotic(0.0f),
	RstAlignmentEvil(0.0f),
	RstAlignmentGood(0.0f),
	RstAlignmentLawful(0.0f),
	RstMental(0.0f),
	RstPoison(0.0f),
	RstBleed(0.0f),
	RstPrecision(0.0f),
	PerceptionModifier(0.0f),
	SkAcrobaticsModifier(0.0f),
	SkArcanaModifier(0.0f),
	SkAthleticsModifier(0.0f),
	SkCraftingModifier(0.0f),
	SkDeceptionModifier(0.0f),
	SkDiplomacyModifier(0.0f),
	SkIntimidationModifier(0.0f),
	SkLore1Modifier(0.0f),
	SkLore2Modifier(0.0f),
	SkMedicineModifier(0.0f),
	SkNatureModifier(0.0f),
	SkOccultismModifier(0.0f),
	SkPerformanceModifier(0.0f),
	SkReligionModifier(0.0f),
	SkSocietyModifier(0.0f),
	SkStealthModifier(0.0f),
	SkSurvivalModifier(0.0f),
	SkThieveryModifier(0.0f),
	SpellAttackRoll(0.0f),
	SpellDifficultyClass(0.0f),
	FeAncestryFeatCount(0.0f),
	FeAncestryFeatLimit(0.0f),
	EncActionPoints(0.0f),
	EncReactionPoints(0.0f),
	TmpDamageIncoming(0.0f)
{
}

void UPF2AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPF2AttributeSet, Experience);
	DOREPLIFETIME(UPF2AttributeSet, AbBoostCount);
	DOREPLIFETIME(UPF2AttributeSet, AbBoostLimit);
	DOREPLIFETIME(UPF2AttributeSet, AbStrength);
	DOREPLIFETIME(UPF2AttributeSet, AbStrengthModifier);
	DOREPLIFETIME(UPF2AttributeSet, AbDexterity);
	DOREPLIFETIME(UPF2AttributeSet, AbDexterityModifier);
	DOREPLIFETIME(UPF2AttributeSet, AbConstitution);
	DOREPLIFETIME(UPF2AttributeSet, AbConstitutionModifier);
	DOREPLIFETIME(UPF2AttributeSet, AbIntelligence);
	DOREPLIFETIME(UPF2AttributeSet, AbIntelligenceModifier);
	DOREPLIFETIME(UPF2AttributeSet, AbWisdom);
	DOREPLIFETIME(UPF2AttributeSet, AbWisdomModifier);
	DOREPLIFETIME(UPF2AttributeSet, AbCharisma);
	DOREPLIFETIME(UPF2AttributeSet, AbCharismaModifier);
	DOREPLIFETIME(UPF2AttributeSet, ClassDifficultyClass);
	DOREPLIFETIME(UPF2AttributeSet, Speed);
	DOREPLIFETIME(UPF2AttributeSet, MaxSpeed);
	DOREPLIFETIME(UPF2AttributeSet, ArmorClass);
	DOREPLIFETIME(UPF2AttributeSet, StFortitudeModifier);
	DOREPLIFETIME(UPF2AttributeSet, StReflexModifier);
	DOREPLIFETIME(UPF2AttributeSet, StWillModifier);
	DOREPLIFETIME(UPF2AttributeSet, HitPoints);
	DOREPLIFETIME(UPF2AttributeSet, MaxHitPoints);
	DOREPLIFETIME(UPF2AttributeSet, RstPhysicalBludgeoning);
	DOREPLIFETIME(UPF2AttributeSet, RstPhysicalPiercing);
	DOREPLIFETIME(UPF2AttributeSet, RstPhysicalSlashing);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyAcid);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyCold);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyFire);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergySonic);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyPositive);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyNegative);
	DOREPLIFETIME(UPF2AttributeSet, RstEnergyForce);
	DOREPLIFETIME(UPF2AttributeSet, RstAlignmentChaotic);
	DOREPLIFETIME(UPF2AttributeSet, RstAlignmentEvil);
	DOREPLIFETIME(UPF2AttributeSet, RstAlignmentGood);
	DOREPLIFETIME(UPF2AttributeSet, RstAlignmentLawful);
	DOREPLIFETIME(UPF2AttributeSet, RstMental);
	DOREPLIFETIME(UPF2AttributeSet, RstPoison);
	DOREPLIFETIME(UPF2AttributeSet, RstBleed);
	DOREPLIFETIME(UPF2AttributeSet, RstPrecision);
	DOREPLIFETIME(UPF2AttributeSet, PerceptionModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkAcrobaticsModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkArcanaModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkAthleticsModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkCraftingModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkDeceptionModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkDiplomacyModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkIntimidationModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkLore1Modifier);
	DOREPLIFETIME(UPF2AttributeSet, SkLore2Modifier);
	DOREPLIFETIME(UPF2AttributeSet, SkMedicineModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkNatureModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkOccultismModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkPerformanceModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkReligionModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkSocietyModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkStealthModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkSurvivalModifier);
	DOREPLIFETIME(UPF2AttributeSet, SkThieveryModifier);
	DOREPLIFETIME(UPF2AttributeSet, SpellAttackRoll);
	DOREPLIFETIME(UPF2AttributeSet, SpellDifficultyClass);
	DOREPLIFETIME(UPF2AttributeSet, FeAncestryFeatCount);
	DOREPLIFETIME(UPF2AttributeSet, FeAncestryFeatLimit);
	DOREPLIFETIME(UPF2AttributeSet, EncActionPoints);
	DOREPLIFETIME(UPF2AttributeSet, EncReactionPoints);
}

void UPF2AttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, Experience, OldValue);
}

void UPF2AttributeSet::OnRep_AbBoostCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbBoostCount, OldValue);
}

void UPF2AttributeSet::OnRep_AbBoostLimit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbBoostLimit, OldValue);
}

void UPF2AttributeSet::OnRep_AbStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbStrength, OldValue);
}

void UPF2AttributeSet::OnRep_AbStrengthModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbStrengthModifier, OldValue);
}

void UPF2AttributeSet::OnRep_AbDexterity(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbDexterity, OldValue);
}

void UPF2AttributeSet::OnRep_AbDexterityModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbDexterityModifier, OldValue);
}

void UPF2AttributeSet::OnRep_AbConstitution(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbConstitution, OldValue);
}

void UPF2AttributeSet::OnRep_AbConstitutionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbConstitutionModifier, OldValue);
}

void UPF2AttributeSet::OnRep_AbIntelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbIntelligence, OldValue);
}

void UPF2AttributeSet::OnRep_AbIntelligenceModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbIntelligenceModifier, OldValue);
}

void UPF2AttributeSet::OnRep_AbWisdom(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbWisdom, OldValue);
}

void UPF2AttributeSet::OnRep_AbWisdomModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbWisdomModifier, OldValue);
}

void UPF2AttributeSet::OnRep_AbCharisma(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbCharisma, OldValue);
}

void UPF2AttributeSet::OnRep_AbCharismaModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, AbCharismaModifier, OldValue);
}

void UPF2AttributeSet::OnRep_ClassDifficultyClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, ClassDifficultyClass, OldValue);
}

void UPF2AttributeSet::OnRep_Speed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, Speed, OldValue);
}

void UPF2AttributeSet::OnRep_MaxSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, MaxSpeed, OldValue);
}

void UPF2AttributeSet::OnRep_ArmorClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, ArmorClass, OldValue);
}

void UPF2AttributeSet::OnRep_StFortitudeModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, StFortitudeModifier, OldValue);
}

void UPF2AttributeSet::OnRep_StReflexModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, StReflexModifier, OldValue);
}

void UPF2AttributeSet::OnRep_StWillModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, StWillModifier, OldValue);
}

void UPF2AttributeSet::OnRep_HitPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, HitPoints, OldValue);
}

void UPF2AttributeSet::OnRep_MaxHitPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, MaxHitPoints, OldValue);
}

void UPF2AttributeSet::OnRep_RstPhysicalBludgeoning(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstPhysicalBludgeoning, OldValue);
}

void UPF2AttributeSet::OnRep_RstPhysicalPiercing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstPhysicalPiercing, OldValue);
}

void UPF2AttributeSet::OnRep_RstPhysicalSlashing(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstPhysicalSlashing, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyAcid(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyAcid, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyCold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyCold, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyFire(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyFire, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergySonic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergySonic, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyPositive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyPositive, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyNegative(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyNegative, OldValue);
}

void UPF2AttributeSet::OnRep_RstEnergyForce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstEnergyForce, OldValue);
}

void UPF2AttributeSet::OnRep_RstAlignmentChaotic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstAlignmentChaotic, OldValue);
}

void UPF2AttributeSet::OnRep_RstAlignmentEvil(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstAlignmentEvil, OldValue);
}

void UPF2AttributeSet::OnRep_RstAlignmentGood(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstAlignmentGood, OldValue);
}

void UPF2AttributeSet::OnRep_RstAlignmentLawful(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstAlignmentLawful, OldValue);
}

void UPF2AttributeSet::OnRep_RstMental(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstMental, OldValue);
}

void UPF2AttributeSet::OnRep_RstPoison(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstPoison, OldValue);
}

void UPF2AttributeSet::OnRep_RstBleed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstBleed, OldValue);
}

void UPF2AttributeSet::OnRep_RstPrecision(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, RstPrecision, OldValue);
}

void UPF2AttributeSet::OnRep_PerceptionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, PerceptionModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkAcrobaticsModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkAcrobaticsModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkArcanaModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkArcanaModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkAthleticsModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkAthleticsModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkCraftingModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkCraftingModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkDeceptionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkDeceptionModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkDiplomacyModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkDiplomacyModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkIntimidationModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkIntimidationModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkLore1Modifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkLore1Modifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkLore2Modifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkLore2Modifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkMedicineModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkMedicineModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkNatureModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkNatureModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkOccultismModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkOccultismModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkPerformanceModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkPerformanceModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkReligionModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkReligionModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkSocietyModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkSocietyModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkStealthModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkStealthModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkSurvivalModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkSurvivalModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SkThieveryModifier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SkThieveryModifier, OldValue);
}

void UPF2AttributeSet::OnRep_SpellAttackRoll(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SpellAttackRoll, OldValue);
}

void UPF2AttributeSet::OnRep_SpellDifficultyClass(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, SpellDifficultyClass, OldValue);
}

void UPF2AttributeSet::OnRep_FeAncestryFeatCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, FeAncestryFeatCount, OldValue);
}

void UPF2AttributeSet::OnRep_FeAncestryFeatLimit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, FeAncestryFeatLimit, OldValue);
}

void UPF2AttributeSet::OnRep_EncActionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, EncActionPoints, OldValue);
}

void UPF2AttributeSet::OnRep_EncReactionPoints(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPF2AttributeSet, EncReactionPoints, OldValue);
}

void UPF2AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UPF2AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle Context   = Data.EffectSpec.GetContext();
	const FGameplayTagContainer*       EventTags = Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	IPF2CharacterInterface* TargetCharacter = PF2GameplayAbilityUtilities::GetEffectTarget(&Data);

	float ValueDelta = 0;

	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		ValueDelta = Data.EvaluatedData.Magnitude;
	}

	if (Data.EvaluatedData.Attribute == this->GetTmpDamageIncomingAttribute())
	{
		this->HandleDamageIncomingChanged(TargetCharacter, Context, ValueDelta, EventTags);
	}
	else if (Data.EvaluatedData.Attribute == this->GetHitPointsAttribute())
	{
		this->HandleHitPointsChanged(TargetCharacter, Context, ValueDelta, EventTags);
	}
}

void UPF2AttributeSet::HandleDamageIncomingChanged(IPF2CharacterInterface*            TargetCharacter,
                                                   const FGameplayEffectContextHandle Context,
                                                   const float                        ValueDelta,
                                                   const FGameplayTagContainer*       EventTags)
{
	const float LocalDamage = this->GetTmpDamageIncoming();

	if (LocalDamage > 0.0f)
	{
		const float OldHitPoints        = this->GetHitPoints();
		const float CurrentMaxHitPoints = this->GetMaxHitPoints();
		const float NewHitPoints        = FMath::Clamp(OldHitPoints - LocalDamage, 0.0f, CurrentMaxHitPoints);

		this->SetTmpDamageIncoming(0.0f);
		this->SetHitPoints(NewHitPoints);

		UE_LOG(
			LogPf2CoreStats,
			VeryVerbose,
			TEXT("Damage: %s - Old HitPoints: %f, Damage: %f, New HitPoints: %f"),
			*(TargetCharacter->GetCharacterName().ToString()),
			OldHitPoints,
			LocalDamage,
			NewHitPoints
		);

		if (TargetCharacter != nullptr)
		{
			const UAbilitySystemComponent* SourceAsc = Context.GetOriginalInstigatorAbilitySystemComponent();

			const FHitResult        HitResult    = UAbilitySystemBlueprintLibrary::EffectContextGetHitResult(Context);
			IPF2CharacterInterface* Instigator   = nullptr;
			AActor*                 DamageSource = nullptr;

			const TWeakObjectPtr<AActor> SourceAvatarActor =
				PF2GameplayAbilityUtilities::GetAvatarActorOfOwner(SourceAsc);

			// Initially, assume that the source actor for damage is the physical damage source actor (e.g., the
			// physical model of the weapon or projectile, such as the mesh for an axe).
			if (SourceAvatarActor.IsValid())
			{
				DamageSource = SourceAvatarActor.Get();
				Instigator   = PF2GameplayAbilityUtilities::GetEffectInstigator(SourceAsc, DamageSource);
			}

			// If we have been given an explicit GE "causer", use that instead of our default.
			if (Context.GetEffectCauser() != nullptr)
			{
				// BUGBUG: Shouldn't the damage source be determined before we determine the instigator? The order in
				//         which we determine the instigator and damage source here matches what the Action RPG sample
				//         from Epic does, but it doesn't seem 100% correct.
				DamageSource = Context.GetEffectCauser();
			}

			TargetCharacter->HandleDamageReceived(LocalDamage, Instigator, DamageSource, EventTags, HitResult);
			TargetCharacter->HandleHitPointsChanged(-LocalDamage, EventTags);
		}
	}
}

void UPF2AttributeSet::HandleHitPointsChanged(IPF2CharacterInterface*            TargetCharacter,
                                              const FGameplayEffectContextHandle Context,
                                              const float                        ValueDelta,
                                              const FGameplayTagContainer*       EventTags)
{
	const float ClampedHitPoints = FMath::Clamp(this->GetHitPoints(), 0.0f, this->GetMaxHitPoints());

	this->SetHitPoints(ClampedHitPoints);

	if (TargetCharacter != nullptr)
	{
		TargetCharacter->HandleHitPointsChanged(ValueDelta, EventTags);
	}
}
