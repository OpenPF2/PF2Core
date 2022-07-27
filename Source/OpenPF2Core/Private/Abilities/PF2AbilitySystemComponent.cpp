// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2AbilitySystemComponent.h"

#include <UObject/ConstructorHelpers.h>

#include "PF2CharacterConstants.h"
#include "PF2CharacterInterface.h"

#include "Utilities/PF2ArrayUtilities.h"
#include "Utilities/PF2EnumUtilities.h"

UPF2AbilitySystemComponent::UPF2AbilitySystemComponent()
{
	const FString DynamicTagsGeFilename =
		PF2CharacterConstants::GetBlueprintPath(*PF2CharacterConstants::GeDynamicTagsName);

	const ConstructorHelpers::FObjectFinder<UClass> DynamicTagsGeFinder(*DynamicTagsGeFilename);

	this->DynamicTagsEffect = DynamicTagsGeFinder.Object;

	for (const auto& Ability : TEnumRange<EPF2CharacterAbilityScoreType>())
	{
		const FString AbilityName = PF2EnumUtilities::ToString(Ability);

		const FString Filename =
			PF2CharacterConstants::GetBlueprintPath(
				FName(FString::Format(*PF2CharacterConstants::GeBlueprintBoostNameFormat, {AbilityName})),
				PF2CharacterConstants::BlueprintSubfolderBoosts
			);

		const ConstructorHelpers::FObjectFinder<UClass> BoostGeFinder(*Filename);

		// Allow boost effects to be looked-up by ability name later.
		this->AbilityBoostEffects.Add(Ability, BoostGeFinder.Object);
	}
}

UAbilitySystemComponent* UPF2AbilitySystemComponent::ToAbilitySystemComponent()
{
	return Cast<UAbilitySystemComponent>(this);
}

void UPF2AbilitySystemComponent::AddPassiveGameplayEffect(const TSubclassOf<UGameplayEffect> Effect)
{
	const FName WeightGroup = PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(Effect);

	this->AddPassiveGameplayEffectWithWeight(WeightGroup, Effect);
}

void UPF2AbilitySystemComponent::AddPassiveGameplayEffectWithWeight(
	const FName                        WeightGroup,
	const TSubclassOf<UGameplayEffect> Effect)
{
	// Special case: If this is the first time a GE from this weight group is being added, and other weight groups are
	// active, let's assume that we want to enable the new weight group.
	if ((this->PassiveGameplayEffects.Num(WeightGroup) == 0) && this->ArePassiveGameplayEffectsActive())
	{
		this->ActivatedWeightGroups.Add(WeightGroup);
	}

	this->InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(WeightGroup, [this, WeightGroup, Effect]
	{
		this->PassiveGameplayEffects.Add(WeightGroup, Effect);
		this->CachedPassiveGameplayEffectsToApply.Empty();

		if (this->ActivatedWeightGroups.Contains(WeightGroup))
		{
			// Activate the new passive GE since it's being put into an active group.
			this->ActivatePassiveGameplayEffect(WeightGroup, Effect);
		}
	});
}

void UPF2AbilitySystemComponent::SetPassiveGameplayEffects(const TMultiMap<FName, TSubclassOf<UGameplayEffect>> Effects)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Effects]
	{
		this->PassiveGameplayEffects = Effects;
		this->CachedPassiveGameplayEffectsToApply.Empty();
	});
}

void UPF2AbilitySystemComponent::RemoveAllPassiveGameplayEffects()
{
	this->DeactivateAllPassiveGameplayEffects();

	this->PassiveGameplayEffects.Empty();
	this->CachedPassiveGameplayEffectsToApply.Empty();
}

void UPF2AbilitySystemComponent::ActivateAllPassiveGameplayEffects()
{
	const TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->GetPassiveGameplayEffectsToApply();

	TSet<FName> AllWeightGroups,
	            InactiveGroups;

	EffectsToApply.GetKeys(AllWeightGroups);

	InactiveGroups = AllWeightGroups.Difference(this->ActivatedWeightGroups);

	for (const auto& InactiveGroup : InactiveGroups)
	{
		this->ActivatePassiveGameplayEffects(InactiveGroup);
	}
}

void UPF2AbilitySystemComponent::DeactivateAllPassiveGameplayEffects()
{
	FGameplayEffectQuery Query;

	Query.EffectSource = this;

	this->RemoveActiveEffects(Query);
	this->ActivatedWeightGroups.Empty();
}

TSet<FName> UPF2AbilitySystemComponent::ActivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup)
{
	const TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->GetPassiveGameplayEffectsToApply();

	TSet<FName> AllWeightGroups,
	            InactiveGroups,
	            ActivatedGroups;

	EffectsToApply.GetKeys(AllWeightGroups);

	InactiveGroups = AllWeightGroups.Difference(this->ActivatedWeightGroups);

	for (const auto& InactiveGroup : InactiveGroups)
	{
		if (StartingWeightGroup.LexicalLess(InactiveGroup) && this->ActivatePassiveGameplayEffects(InactiveGroup))
		{
			ActivatedGroups.Add(InactiveGroup);
		}
	}

	return ActivatedGroups;
}

TSet<FName> UPF2AbilitySystemComponent::DeactivatePassiveGameplayEffectsAfter(const FName StartingWeightGroup)
{
	TSet<FName> DeactivatedGroups;

	// We have to make a copy of the set because we'll be modifying it in the loop.
	TSet<FName> WeightGroups = this->ActivatedWeightGroups;

	for (const FName& ActiveGroup : WeightGroups)
	{
		if (StartingWeightGroup.LexicalLess(ActiveGroup) && this->DeactivatePassiveGameplayEffects(ActiveGroup))
		{
			DeactivatedGroups.Add(ActiveGroup);
		}
	}

	return DeactivatedGroups;
}

bool UPF2AbilitySystemComponent::ActivatePassiveGameplayEffects(const FName WeightGroup)
{
	if (this->ActivatedWeightGroups.Contains(WeightGroup))
	{
		return false;
	}
	else
	{
		const TMultiMap<FName, TSubclassOf<UGameplayEffect>> AllEffects   = this->GetPassiveGameplayEffectsToApply();
		TArray<TSubclassOf<UGameplayEffect>>                 GroupEffects;

		AllEffects.MultiFind(WeightGroup, GroupEffects, true);

		for (const auto& GameplayEffect : GroupEffects)
		{
			this->ActivatePassiveGameplayEffect(WeightGroup, GameplayEffect);
		}

		this->ActivatedWeightGroups.Add(WeightGroup);

		return true;
	}
}

bool UPF2AbilitySystemComponent::DeactivatePassiveGameplayEffects(const FName WeightGroup)
{
	if (!this->ActivatedWeightGroups.Contains(WeightGroup))
	{
		return false;
	}
	else
	{
		FGameplayEffectQuery Query;
		int32                NumRemoved;

		Query.EffectSource = this;

		Query.EffectTagQuery =
			FGameplayTagQuery::MakeQuery_MatchAnyTags(
				FGameplayTagContainer(PF2GameplayAbilityUtilities::GetTag(WeightGroup))
			);

		NumRemoved = this->RemoveActiveEffects(Query);

		this->ActivatedWeightGroups.Remove(WeightGroup);

		return (NumRemoved > 0);
	}
}

void UPF2AbilitySystemComponent::AddDynamicTag(const FGameplayTag Tag)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tag]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Adding a dynamic tag ('%s') to ASC on character ('%s')."),
			*(Tag.ToString()),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags.AddTag(Tag);
	});
}

void UPF2AbilitySystemComponent::AppendDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Adding dynamic tags ('%s') to ASC on character ('%s')."),
			*(Tags.ToString()),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags.AppendTags(Tags);
	});
}

void UPF2AbilitySystemComponent::SetDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Setting all dynamic tags ('%s') in ASC on character ('%s')."),
			*(Tags.ToString()),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags = Tags;
	});
}

void UPF2AbilitySystemComponent::RemoveDynamicTag(const FGameplayTag Tag)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tag]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing a dynamic tag ('%s') from ASC on character ('%s')."),
			*(Tag.ToString()),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags.RemoveTag(Tag);
	});
}

void UPF2AbilitySystemComponent::RemoveDynamicTags(const FGameplayTagContainer Tags)
{
	this->InvokeAndReapplyAllPassiveGEs([this, Tags]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing dynamic tags ('%s') from ASC on character ('%s')."),
			*(Tags.ToString()),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags.RemoveTags(Tags);
	});
}

void UPF2AbilitySystemComponent::RemoveAllDynamicTags()
{
	this->InvokeAndReapplyAllPassiveGEs([this]
	{
		UE_LOG(
			LogPf2Core,
			VeryVerbose,
			TEXT("Removing all dynamic tags from ASC on character ('%s')."),
			*(this->GetOwnerActor()->GetName())
		);

		this->DynamicTags.Reset();
	});
}

FGameplayTagContainer UPF2AbilitySystemComponent::GetActiveGameplayTags() const
{
	FGameplayTagContainer Tags;

	Tags.Reset();

	this->GetOwnedGameplayTags(Tags);

	return Tags;
}

FORCEINLINE int32 UPF2AbilitySystemComponent::GetCharacterLevel() const
{
	IPF2CharacterInterface* OwningCharacter = Cast<IPF2CharacterInterface>(this->GetOwnerActor());

	if (OwningCharacter == nullptr)
	{
		return 1;
	}
	else
	{
		return OwningCharacter->GetCharacterLevel();
	}
}

TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> UPF2AbilitySystemComponent::GetAbilityScoreValues() const
{
	const UPF2AttributeSet* AttributeSet =
		Cast<UPF2AttributeSet>(this->GetAttributeSubobject(UPF2AttributeSet::StaticClass()));

	const TMap<EPF2CharacterAbilityScoreType, FPF2AttributeModifierSnapshot> Values =
		{
			{
				EPF2CharacterAbilityScoreType::AbStrength,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbStrength.GetCurrentValue(),
					AttributeSet->AbStrengthModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbDexterity,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbDexterity.GetCurrentValue(),
					AttributeSet->AbDexterityModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbConstitution,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbConstitution.GetCurrentValue(),
					AttributeSet->AbConstitutionModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbIntelligence,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbIntelligence.GetCurrentValue(),
					AttributeSet->AbIntelligenceModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbWisdom,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbWisdom.GetCurrentValue(),
					AttributeSet->AbWisdomModifier.GetCurrentValue()
				)
			},
			{
				EPF2CharacterAbilityScoreType::AbCharisma,
				FPF2AttributeModifierSnapshot(
					AttributeSet->AbCharisma.GetCurrentValue(),
					AttributeSet->AbCharismaModifier.GetCurrentValue()
				)
			},
		};

	return Values;
}

TArray<UPF2AbilityBoostBase*> UPF2AbilitySystemComponent::GetPendingAbilityBoosts() const
{
	TArray<UPF2AbilityBoostBase*> MatchingGameplayAbilities;
	TArray<FGameplayAbilitySpec*> MatchingGameplayAbilitySpecs;

	this->GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		FGameplayTagContainer(PF2GameplayAbilityUtilities::GetTag(FName("GameplayAbility.Type.AbilityBoost"))),
		MatchingGameplayAbilitySpecs,
		false
	);

	MatchingGameplayAbilities =
		PF2ArrayUtilities::Map<UPF2AbilityBoostBase*>(
			MatchingGameplayAbilitySpecs,
			[](const FGameplayAbilitySpec* AbilitySpec)
			{
				return Cast<UPF2AbilityBoostBase>(AbilitySpec->Ability);
			}
		);

	return MatchingGameplayAbilities;
}

void UPF2AbilitySystemComponent::ApplyAbilityBoost(const EPF2CharacterAbilityScoreType TargetAbilityScore)
{
	const TSubclassOf<UGameplayEffect> BoostEffect = this->AbilityBoostEffects[TargetAbilityScore];

	// Allow boost GE to override the default weight group.
	const FName WeightGroup =
		PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(
			BoostEffect,
			PF2CharacterConstants::GeWeightGroups::AbilityBoosts
		);

	UE_LOG(
		LogPf2Core,
		VeryVerbose,
		TEXT("Applying a boost to ability ('%s') through ASC for character ('%s') via GE ('%s')."),
		*(PF2EnumUtilities::ToString(TargetAbilityScore)),
		*(this->GetOwnerActor()->GetName()),
		*(BoostEffect->GetName())
	);

	this->AddPassiveGameplayEffectWithWeight(WeightGroup, BoostEffect);
}

TMultiMap<FName, TSubclassOf<UGameplayEffect>> UPF2AbilitySystemComponent::GetPassiveGameplayEffectsToApply()
{
	if (this->CachedPassiveGameplayEffectsToApply.Num() == 0)
	{
		this->CachedPassiveGameplayEffectsToApply = this->BuildPassiveGameplayEffectsToApply();
	}

	return this->CachedPassiveGameplayEffectsToApply;
}

TMultiMap<FName, TSubclassOf<UGameplayEffect>> UPF2AbilitySystemComponent::BuildPassiveGameplayEffectsToApply() const
{
	TMultiMap<FName, TSubclassOf<UGameplayEffect>> EffectsToApply = this->PassiveGameplayEffects;

	// Add a pseudo-GE for the dynamic tags.
	EffectsToApply.Add(PF2CharacterConstants::GeWeightGroups::InitializeBaseStats, this->DynamicTagsEffect);

	// Ensure Passive GEs are always evaluated in weight order.
	EffectsToApply.KeyStableSort(FNameLexicalLess());

	return EffectsToApply;
}

void UPF2AbilitySystemComponent::ActivatePassiveGameplayEffect(
	const FName                        WeightGroup,
	const TSubclassOf<UGameplayEffect> GameplayEffect)
{
	FGameplayEffectContextHandle EffectContext = this->MakeEffectContext();
	FGameplayEffectSpecHandle    NewHandle;
	FGameplayEffectSpec*         GameplayEffectSpec;

	EffectContext.AddSourceObject(this);

	NewHandle = this->MakeOutgoingSpec(
		GameplayEffect,
		this->GetCharacterLevel(),
		EffectContext
	);

	GameplayEffectSpec = NewHandle.Data.Get();

	// Ensure that the GE spec is tagged with its weight no matter how the weight was set (either through API or through
	// a tag in the InheritableGameplayEffectTags field on the GE definition class itself). Without this, only the tag
	// from the GE definition spec would pass through.
	GameplayEffectSpec->DynamicAssetTags.AddTag(PF2GameplayAbilityUtilities::GetTag(WeightGroup));

	// Special case: If the GE being activated is our "dummy" GE for dynamic tags, apply tags to it.
	//
	// TODO: Find a different way to accomplish this without a GE. This feels very much like a kludge.
	if (GameplayEffect->GetName() == PF2CharacterConstants::GeDynamicTagsClassName)
	{
		GameplayEffectSpec->DynamicGrantedTags.AppendTags(this->DynamicTags);
	}

	if (NewHandle.IsValid())
	{
		this->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpec, this);
	}
}

template <typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyAllPassiveGEs(const Func Callable)
{
	const bool bWasActive = this->ArePassiveGameplayEffectsActive();

	if (bWasActive)
	{
		this->DeactivateAllPassiveGameplayEffects();
	}

	Callable();

	if (bWasActive)
	{
		this->ActivateAllPassiveGameplayEffects();
	}
}

template<typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(
	const TSubclassOf<UGameplayEffect> Effect,
	const Func Callable)
{
	FName WeightGroup = PF2GameplayAbilityUtilities::GetWeightGroupOfGameplayEffect(Effect);

	this->InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(WeightGroup, Callable);
}


template <typename Func>
void UPF2AbilitySystemComponent::InvokeAndReapplyPassiveGEsInSubsequentWeightGroups(
	const FName WeightGroup,
	const Func Callable)
{
	// NOTE: If the group we are affecting isn't active, we don't bother to re-apply subsequent groups because they
	// won't be affected.
	const bool bSubsequentGroupsWereActive =
		this->ActivatedWeightGroups.Contains(WeightGroup) &&
		(this->DeactivatePassiveGameplayEffectsAfter(WeightGroup).Num() != 0);

	Callable();

	if (bSubsequentGroupsWereActive)
	{
		this->ActivatePassiveGameplayEffectsAfter(WeightGroup);
	}
}
