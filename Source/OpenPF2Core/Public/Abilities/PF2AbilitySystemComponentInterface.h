﻿#pragma once

#include <GameplayTagContainer.h>

#include "PF2AbilitySystemComponentInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPF2AbilitySystemComponentInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * An interface for OpenPF2-compatible Ability System Components.
 *
 * This interface extends ASCs to support additional, dynamic logic for adding and removing passive GEs and replicated
 * tags, which are commonly generated from settings added to characters and other objects that have an ASC by game
 * designers using Blueprint.
 *
 * Epic places logic like this on the character base object, but that isn't the ideal place because the logic has to
 * interact closely with the ASC on the character to have an effect. So, in OpenPF2, that logic lives in ASCs instead to
 * cut down on the number of concerns character base classes are managing.
 */
class OPENPF2CORE_API IPF2AbilitySystemComponentInterface
{
    GENERATED_BODY()

public:
	/**
	 * Gets whether passively-applied Gameplay Effects are currently active on this ASC.
	 */
	UFUNCTION()
	virtual bool ArePassiveGameplayEffectsActive() = 0;

	/**
	 * Adds a passively-applied Gameplay Effects with the current weight to this ASC.
	 *
	 * Different instances of the same type of GE can be added multiple times, even with the same weight. Each addition
	 * adds an additional entry for that GE under the given weight.
	 *
	 * @param Weight
	 *	The weight of that GE.
	 * @param Effect
	 *	The gameplay effect to add under the given weight.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void AddPassiveGameplayEffect(const int32 Weight, const TSubclassOf<UGameplayEffect> Effect) = 0;

	/**
	 * Sets all of the passive Gameplay Effects on this ASC to the given set.
	 *
	 * If passive GEs are currently active on this ASC, they will be removed and the new GEs will be applied as part of
	 * this call.
	 *
	 * @param Effects
	 *	The list of Gameplay Effects (GEs) to always passively apply to this ASC. Each value must be a gameplay effect
	 *	and the key must be the weight of that GE. The weight controls the order that all GEs are applied. Lower weights
	 *	are applied earlier than higher weights.
	 */
	virtual void SetPassiveGameplayEffects(const TMultiMap<int32, TSubclassOf<UGameplayEffect>> Effects) = 0;

	/**
	 * Clears all of the passive Gameplay Effects on this ASC.
	 *
	 * If passive GEs are currently active on this ASC, they will be removed and deactivated as part of this call.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveAllPassiveGameplayEffects() = 0;

	/**
	 * Activates Gameplay Effects that should remain passively applied on this ASC until deactivated.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ActivatePassiveGameplayEffects() = 0;

	/**
	 * Removes all passive Gameplay Effects that were previously activated on this ASC.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void DeactivatePassiveGameplayEffects() = 0;

	/**
	 * Applies a tag to this ASC that is otherwise not granted by a GE.
	 *
	 * This can be used to apply a replicated tag that is specific to a particular character instance, such as age,
	 * size, skill proficiency, etc. If passive GEs are currently active on this ASC, they will be re-applied when this
	 * method is called. Consequently, calling AppendDynamicTags() is preferred over this method when there are multiple
	 * tags that should be applied at the same time, to avoid unnecessary overhead from re-applying all passive GEs.
	 *
	 * @param Tag
	 *	The tag to apply to this Ability System Component.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void AddDynamicTag(const FGameplayTag Tag) = 0;

	/**
	 * Applies multiple replicated tags to this ASC that are otherwise not granted by a GE.
	 *
	 * This can be used to apply replicated tags that are specific to a particular character instance, such as age,
	 * size, skill proficiency, etc. If passive GEs are currently active on this ASC, they will be re-applied when this
	 * method is called. Consequently, calling this method is preferred over AddDynamicTag() when there are multiple
	 * tags that should be applied at the same time, to avoid unnecessary overhead from re-applying all passive GEs.
	 *
	 * @param Tags
	 *	The tag to apply to this Ability System Component.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void AppendDynamicTags(const FGameplayTagContainer Tags) = 0;

	/**
	 * Sets all of the replicated tags in this ASC that are otherwise not granted by a GE.
	 *
	 * This can be used to apply replicated tags that are specific to a particular character instance, such as age,
	 * size, skill proficiency, etc. If passive GEs are currently active on this ASC, they will be re-applied when this
	 * method is called.
	 *
	 * @param Tags
	 *	The new collection of all the tags that should be applied on this Ability System Component.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetDynamicTags(const FGameplayTagContainer Tags) = 0;

	/**
	 * Removes a tag from this ASC that was previously added with AddDynamicTag() or AppendDynamicTags().
	 *
	 * This can be used to remove a tag that is specific to a particular character instance, such as age, size, skill
	 * proficiency, etc. If passive GEs are currently active on this ASC, they will be re-applied when this method is
	 * called. Consequently, calling RemoveDynamicTags() is preferred over this method when there are multiple tags that
	 * should be removed at the same time, to avoid unnecessary overhead from re-applying all passive GEs.
	 *
	 * @param Tag
	 *	The tag to remove from this Ability System Component.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveDynamicTag(const FGameplayTag Tag) = 0;

	/**
	 * Removes multiple tags from this ASC that were previously added with AddDynamicTag() or AppendDynamicTags().
	 *
	 * This can be used to remove tags that are specific to a particular character instance, such as age, size, skill
	 * proficiency, etc. If passive GEs are currently active on this ASC, they will be re-applied when this method is
	 * called. Consequently, calling this method is preferred over RemoveDynamicTag() when there are multiple tags that
	 * should be applied at the same time, to avoid unnecessary overhead from re-applying all passive GEs.
	 *
	 * @param Tags
	 *	The tags to remove from this Ability System Component.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveDynamicTags(const FGameplayTagContainer Tags) = 0;

	/**
	 * Clears all of the dynamic tags that were previously added to this ASC.
	 *
	 * If passive GEs are currently active on this ASC, they will be re-applied when this method is called.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void RemoveAllDynamicTags() = 0;
};