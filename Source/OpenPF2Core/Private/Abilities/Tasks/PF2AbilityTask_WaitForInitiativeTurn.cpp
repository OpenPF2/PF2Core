// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/Tasks/PF2AbilityTask_WaitForInitiativeTurn.h"

#include "OpenPF2Core.h"
#include "PF2CharacterInterface.h"

#include "Abilities/PF2AbilitySystemComponentInterface.h"
#include "Abilities/PF2ActionQueueResult.h"
#include "Abilities/PF2GameplayAbilityInterface.h"

#include "GameModes/PF2GameModeInterface.h"

#include "Utilities/PF2EnumUtilities.h"
#include "Utilities/PF2InterfaceUtilities.h"
#include "Utilities/PF2LogUtilities.h"

UPF2AbilityTask_WaitForInitiativeTurn* UPF2AbilityTask_WaitForInitiativeTurn::CreateWaitInitiativeTurn(
	UGameplayAbility* OwningAbility,
	const FName       TaskInstanceName,
	const FText       ActionName,
	const FSlateBrush ActionIcon)
{
	UPF2AbilityTask_WaitForInitiativeTurn* Task =
		NewAbilityTask<UPF2AbilityTask_WaitForInitiativeTurn>(OwningAbility, TaskInstanceName);

	Task->ActionName = ActionName;
	Task->ActionIcon = ActionIcon;

	return Task;
}

UPF2AbilityTask_WaitForInitiativeTurn* UPF2AbilityTask_WaitForInitiativeTurn::CreateWaitInitiativeTurnWithTags(
	UGameplayAbility*            OwningAbility,
	const FName                  TaskInstanceName,
	const FText                  ActionName,
	const FSlateBrush            ActionIcon,
	const FGameplayTagContainer& SourceTags,
	const FGameplayTagContainer& TargetTags)
{
	UPF2AbilityTask_WaitForInitiativeTurn* Task =
		NewAbilityTask<UPF2AbilityTask_WaitForInitiativeTurn>(OwningAbility, TaskInstanceName);

	Task->ActionName = ActionName;
	Task->ActionIcon = ActionIcon;
	Task->SourceTags = &SourceTags;
	Task->TargetTags = &TargetTags;

	return Task;
}

void UPF2AbilityTask_WaitForInitiativeTurn::Activate()
{
	IPF2CharacterInterface* PF2Character = Cast<IPF2CharacterInterface>(this->GetOwnerActor());

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Activate() called on ability task ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetName())
	);

	if ((PF2Character != nullptr) && this->HasAbility())
	{
		this->WaitingCharacter = PF2Character;

		if (this->IsPredictingClient())
		{
			this->Activate_Client();
		}
		else
		{
			this->Activate_Server(PF2Character);
		}
	}
}

void UPF2AbilityTask_WaitForInitiativeTurn::ExternalCancel()
{
	if (!this->WasActivatedOrCanceled())
	{
		this->ActivationOutcome = EPF2AbilityActivationResult::Cancelled;

		if (this->ShouldBroadcastAbilityTaskDelegates())
		{
			this->OnCancelled.Broadcast();
		}

		Super::ExternalCancel();
	}
}

void UPF2AbilityTask_WaitForInitiativeTurn::OnDestroy(bool bAbilityEnded)
{
	if ((this->WaitingCharacter != nullptr) && (this->GameMode != nullptr) && !this->WasActivatedOrCanceled())
	{
		const TScriptInterface<IPF2CharacterInterface> CharacterScriptInterface =
			this->WaitingCharacter.ToScriptInterface();

		const TScriptInterface<IPF2QueuedActionInterface> ThisScriptInterface =
			PF2InterfaceUtilities::ToScriptInterface<IPF2QueuedActionInterface>(this);

		this->GameMode->CancelActionQueuedForInitiativeTurn(CharacterScriptInterface, ThisScriptInterface);
	}
}

FString UPF2AbilityTask_WaitForInitiativeTurn::GetIdForLogs() const
{
	check(this->HasAbility());

	// ReSharper disable CppRedundantParentheses
	return FString::Format(
		TEXT("{0}[{1}.{2}]"),
		{
			*(this->GetActionName().ToString()),
			*(this->Ability->GetName()),
			*(this->GetName())
		}
	);
}

FText UPF2AbilityTask_WaitForInitiativeTurn::GetActionName() const
{
	return this->ActionName;
}

FSlateBrush UPF2AbilityTask_WaitForInitiativeTurn::GetActionIcon() const
{
	return this->ActionIcon;
}

EPF2AbilityActivationResult UPF2AbilityTask_WaitForInitiativeTurn::PerformAction()
{
	EPF2AbilityActivationResult Result = this->ActivationOutcome;

	if (this->HasAbility() && !this->IsPendingKill() && !this->WasActivatedOrCanceled())
	{
		if (this->CanAbilityProceed())
		{
			IPF2GameplayAbilityInterface* Pf2Ability = this->GetOwningPf2Ability();

			UE_LOG(
				LogPf2CoreAbilities,
				VeryVerbose,
				TEXT("[%s] Performing action ('%s') for character ('%s')."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(this->GetIdForLogs()),
				((this->WaitingCharacter != nullptr) ? *(this->WaitingCharacter->GetIdForLogs()) : TEXT("UNK"))
			);

			if (Pf2Ability != nullptr)
			{
				Pf2Ability->OnDequeued();
			}

			if (this->ShouldBroadcastAbilityTaskDelegates())
			{
				this->OnReadyToAct.Broadcast();
			}

			if (this->IsForRemoteClient() && this->HasAsc())
			{
				// Allow the client to proceed with its predicted copy of the ability.
				this->AbilitySystemComponent->ClientSetReplicatedEvent(
					EAbilityGenericReplicatedEvent::GenericSignalFromServer,
					this->GetAbilitySpecHandle(),
					this->GetActivationPredictionKey()
				);
			}

			Result = EPF2AbilityActivationResult::Activated;

			this->EndTask();
		}
		else
		{
			UE_LOG(
				LogPf2CoreAbilities,
				VeryVerbose,
				TEXT("[%s] Action ('%s') blocked for character ('%s')."),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(this->GetIdForLogs()),
				((this->WaitingCharacter != nullptr) ? *(this->WaitingCharacter->GetIdForLogs()) : TEXT("UNK"))
			);

			Result = EPF2AbilityActivationResult::Blocked;
		}

		this->ActivationOutcome = Result;
	}

	return Result;
}

void UPF2AbilityTask_WaitForInitiativeTurn::CancelAction()
{
	this->ExternalCancel();
}

void UPF2AbilityTask_WaitForInitiativeTurn::Activate_Client()
{
	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Activate_Client() called on ability task ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetName())
	);

	if (this->HasAsc())
	{
		FScopedPredictionWindow       ScopedPrediction = FScopedPredictionWindow(this->AbilitySystemComponent, true);
		IPF2GameplayAbilityInterface* Pf2Ability       = this->GetOwningPf2Ability();

		if (Pf2Ability != nullptr)
		{
			Pf2Ability->OnQueued();
		}

		this->CallOrAddReplicatedDelegate(
			EAbilityGenericReplicatedEvent::GenericSignalFromServer,
			FSimpleMulticastDelegate::FDelegate::CreateUObject(
				this,
				&UPF2AbilityTask_WaitForInitiativeTurn::OnPerformAction_Client
			)
		);
	}
}

void UPF2AbilityTask_WaitForInitiativeTurn::Activate_Server(IPF2CharacterInterface* PF2Character)
{
	const UWorld* const World = this->GetWorld();

	UE_LOG(
		LogPf2CoreAbilities,
		VeryVerbose,
		TEXT("[%s] Activate_Server() called on ability task ('%s')."),
		*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
		*(this->GetName())
	);

	if (World != nullptr)
	{
		IPF2GameModeInterface* PF2GameMode = Cast<IPF2GameModeInterface>(World->GetAuthGameMode());

		if (PF2GameMode != nullptr)
		{
			TScriptInterface<IPF2CharacterInterface> CharacterScriptInterface =
				PF2InterfaceUtilities::ToScriptInterface<IPF2CharacterInterface>(PF2Character);

			TScriptInterface<IPF2QueuedActionInterface> ThisScriptInterface =
				PF2InterfaceUtilities::ToScriptInterface<IPF2QueuedActionInterface>(this);

			EPF2ActionQueueResult QueueResult;

			this->GameMode = PF2GameMode;

			PF2GameMode->QueueActionForInitiativeTurn(CharacterScriptInterface, ThisScriptInterface, QueueResult);

			UE_LOG(
				LogPf2CoreAbilities,
				VeryVerbose,
				TEXT("[%s] Result of queueing action ('%s') was: '%s'"),
				*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
				*(this->GetIdForLogs()),
				*(PF2EnumUtilities::ToString(QueueResult))
			);

			switch (QueueResult)
			{
			case EPF2ActionQueueResult::Refused:
				// The MoPRS refused to allow this action at all (rare).
				this->CancelAction();
				break;

			case EPF2ActionQueueResult::ExecutedImmediately:
				// The MoPRS already executed the action without any queueing, so we have nothing left to do.
				break;

			default:
			case EPF2ActionQueueResult::Queued:
				IPF2GameplayAbilityInterface* Pf2Ability = this->GetOwningPf2Ability();

				if (Pf2Ability != nullptr)
				{
					// The MoPRS queued the action for later execution. Give the ability the chance to temporarily
					// unblock other abilities that would otherwise be incompatible with it.
					Pf2Ability->OnQueued();
				}

				this->OnQueued.Broadcast();
				this->SetWaitingOnRemotePlayerData();
				break;
			}
		}
	}
}

void UPF2AbilityTask_WaitForInitiativeTurn::OnPerformAction_Client()
{
	if (this->HasAsc())
	{
		this->AbilitySystemComponent->ConsumeGenericReplicatedEvent(
			EAbilityGenericReplicatedEvent::GenericSignalFromServer,
			this->GetAbilitySpecHandle(),
			this->GetActivationPredictionKey()
		);
	}

	this->PerformAction();
}

bool UPF2AbilityTask_WaitForInitiativeTurn::CanAbilityProceed() const
{
	bool                             bCanProceed;
	const FGameplayAbilitySpecHandle AbilitySpecHandle = this->Ability->GetCurrentAbilitySpecHandle();
	const FGameplayAbilityActorInfo  ActorInfo         = this->Ability->GetActorInfo();
	IPF2GameplayAbilityInterface*    Pf2Ability        = this->GetOwningPf2Ability();

	if (Pf2Ability != nullptr)
	{
		// Prevent blocking ourselves.
		Pf2Ability->ForceSuspendBlocking();
	}

	bCanProceed = this->Ability->CanActivateAbility(AbilitySpecHandle, &ActorInfo, this->SourceTags, this->TargetTags);

	if (Pf2Ability != nullptr)
	{
		Pf2Ability->ForceResumeBlocking();
	}

	return bCanProceed;
}

IPF2GameplayAbilityInterface* UPF2AbilityTask_WaitForInitiativeTurn::GetOwningPf2Ability() const
{
	IPF2GameplayAbilityInterface* Pf2Ability;

	ensure(this->HasAbility());

	Pf2Ability = Cast<IPF2GameplayAbilityInterface>(this->Ability);

	if (Pf2Ability == nullptr)
	{
		UE_LOG(
			LogPf2CoreAbilities,
			Warning,
			TEXT("[%s] %s invoked on a non-PF2 GA. The ability may not behave as expected if queued during encounters."),
			*(PF2LogUtilities::GetHostNetId(this->GetWorld())),
			*(this->GetClass()->GetName()),
			*(this->GetIdForLogs())
		);
	}

	return Pf2Ability;
}
