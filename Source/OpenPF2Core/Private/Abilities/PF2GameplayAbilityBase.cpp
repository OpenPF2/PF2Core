// OpenPF2 for UE Game Logic, Copyright 2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Abilities/PF2GameplayAbilityBase.h"
#include "Utilities/PF2LogUtilities.h"

UTexture2D* UPF2GameplayAbilityBase::GetAbilityIcon() const
{
	return this->Icon;
}

FText UPF2GameplayAbilityBase::GetAbilityLabel() const
{
	return this->Label;
}

FText UPF2GameplayAbilityBase::GetAbilityDescription() const
{
	return this->Description;
}

FName UPF2GameplayAbilityBase::GetDefaultInputActionMapping() const
{
	return this->DefaultInputActionMapping;
}

FString UPF2GameplayAbilityBase::GetIdForLogs() const
{
	return this->GetName();
}
