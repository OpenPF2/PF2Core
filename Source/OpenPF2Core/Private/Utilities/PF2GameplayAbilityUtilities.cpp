// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Utilities/PF2GameplayAbilityUtilities.h"

#include <GameplayEffectExtension.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>

#include "PF2CharacterInterface.h"
#include "Abilities/PF2CharacterAbilitySystemInterface.h"

/**
 * Utility logic for working with Gameplay Abilities.
 */
namespace PF2GameplayAbilityUtilities
{
	FGameplayEffectAttributeCaptureDefinition BuildSourceCaptureFor(const FGameplayAttribute Attribute)
	{
		FGameplayEffectAttributeCaptureDefinition CaptureDefinition;

		CaptureDefinition.AttributeToCapture = Attribute;
		CaptureDefinition.AttributeSource    = EGameplayEffectAttributeCaptureSource::Source;
		CaptureDefinition.bSnapshot          = false;

		return CaptureDefinition;
	}

	FName GetWeightGroupOfGameplayEffect(const TSubclassOf<UGameplayEffect> GameplayEffect, const FName DefaultWeight)
	{
		FName                  WeightGroup;
		const FGameplayTag     WeightTagParent = GetTag(FName(TEXT("GameplayEffect.WeightGroup")));
		const UGameplayEffect* Effect          = GameplayEffect.GetDefaultObject();

		const FGameplayTagContainer WeightTags =
			Effect->InheritableGameplayEffectTags.CombinedTags.Filter(FGameplayTagContainer(WeightTagParent));

		if (WeightTags.IsEmpty())
		{
			WeightGroup = DefaultWeight;
		}
		else
		{
			const FGameplayTag WeightTag = WeightTags.First();

			checkf(
				WeightTags.Num() < 2,
				TEXT("A Gameplay Effect can only have a single weight group assigned (this GE has been assigned '%d' weight groups)."),
				WeightTags.Num()
			);

			checkf(
				WeightTag != WeightTagParent,
				TEXT("Parent tag of weight groups ('%s') cannot be used as a weight group "),
				*WeightTagParent.ToString()
			);

			WeightGroup = WeightTag.GetTagName();
		}

		return WeightGroup;
	}

	FORCEINLINE IPF2CharacterAbilitySystemInterface* GetCharacterAbilitySystemComponent(
		const FGameplayAbilityActorInfo* ActorInfo)
	{
		IPF2CharacterAbilitySystemInterface* CharacterAsc;
		UAbilitySystemComponent*             AbilitySystemComponent = GetAbilitySystemComponent(ActorInfo);

		CharacterAsc = Cast<IPF2CharacterAbilitySystemInterface>(AbilitySystemComponent);
		check(CharacterAsc != nullptr);

		return CharacterAsc;
	}

	FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponent(
		const FGameplayAbilityActorInfo* ActorInfo)
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
		check(AbilitySystemComponent != nullptr);

		return AbilitySystemComponent;
	}

	FORCEINLINE const UPF2AttributeSet* GetAttributeSet(
		const FGameplayAbilityActorInfo* ActorInfo)
	{
		const UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent(ActorInfo);
		const UPF2AttributeSet*        AttributeSet;

		AttributeSet = AbilitySystemComponent->GetSet<UPF2AttributeSet>();
		check(AttributeSet != nullptr);

		return AttributeSet;
	}

	IPF2CharacterInterface* GetEffectTarget(const FGameplayEffectModCallbackData* Data)
	{
		IPF2CharacterInterface*        TargetCharacter = nullptr;
		const UAbilitySystemComponent& TargetAsc       = Data->Target;
		const TWeakObjectPtr<AActor>   TargetActor     = GetAvatarActorOfOwner(&TargetAsc);

		if (TargetActor.IsValid())
		{
			TargetCharacter = Cast<IPF2CharacterInterface>(TargetActor.Get());
		}

		return TargetCharacter;
	}

	IPF2CharacterInterface* GetEffectInstigator(const UAbilitySystemComponent* SourceAsc, AActor* DamageSource)
	{
		IPF2CharacterInterface* Instigator;
		AController*            SourceController = SourceAsc->AbilityActorInfo->PlayerController.Get();

		if ((SourceController == nullptr) && (DamageSource != nullptr))
		{
			const APawn* Pawn = Cast<APawn>(DamageSource);

			if (Pawn != nullptr)
			{
				SourceController = Pawn->GetController();
			}
		}

		if (SourceController != nullptr)
		{
			Instigator = Cast<IPF2CharacterInterface>(SourceController->GetPawn());
		}
		else
		{
			Instigator = Cast<IPF2CharacterInterface>(DamageSource);
		}

		return Instigator;
	}

	TWeakObjectPtr<AActor> GetAvatarActorOfOwner(const UAbilitySystemComponent* Asc)
	{
		TWeakObjectPtr<AActor>                AvatarActor;
		TSharedPtr<FGameplayAbilityActorInfo> ActorInfo = nullptr;

		if (Asc != nullptr)
		{
			ActorInfo = Asc->AbilityActorInfo;
		}

		if (ActorInfo.IsValid())
		{
			AvatarActor = ActorInfo->AvatarActor;
		}

		return AvatarActor;
	}
}
