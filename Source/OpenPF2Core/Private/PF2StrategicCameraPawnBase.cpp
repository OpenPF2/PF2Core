// OpenPF2 for UE Game Logic, Copyright 2021-2022, Guy Elsmore-Paddock. All Rights Reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// This code is adapted from the "Real-Time Strategy Plugin for Unreal Engine 4" by Nick Pruehs, provided under the MIT
// License. Copyright (c) 2017 Nick Pruehs.
//

#include "PF2StrategicCameraPawnBase.h"

#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>

APF2StrategicCameraPawnBase::APF2StrategicCameraPawnBase() : Super()
{
	this->CameraSpeed         = 1000.0f;
	this->CameraZoomSpeed     = 4000.0f;
	this->CameraTiltZoomSpeed = 2.0f;

	this->MinCameraDistance = 500.0f;
	this->MaxCameraDistance = 2500.0f;
}

void APF2StrategicCameraPawnBase::Tick(const float DeltaSeconds)
{
	FVector           Location;
	UCameraComponent* PlayerPawnCamera;
	const float       CameraRightLeftInput  = FMath::Clamp(this->CameraRightLeftAxisValue, -1.0f, +1.0f);
	const float       CameraUpDownAxisInput = FMath::Clamp(this->CameraUpDownAxisValue,    -1.0f, +1.0f);
	const float       TiltZoomInput         = FMath::Clamp(this->CameraTiltZoomAxisValue,  -1.0f, +1.0f),
	                  TiltZoomValue         = this->CameraTiltZoomSpeed * TiltZoomInput * DeltaSeconds;

	Super::Tick(DeltaSeconds);

	// Apply lateral input.
    Location = this->GetActorLocation();
    Location += FVector::RightVector   * this->CameraSpeed * CameraRightLeftInput  * DeltaSeconds;
    Location += FVector::ForwardVector * this->CameraSpeed * CameraUpDownAxisInput * DeltaSeconds;

	this->SetActorLocation(Location);

    // Apply zoom input.
    PlayerPawnCamera = this->FindComponentByClass<UCameraComponent>();

    if (IsValid(PlayerPawnCamera))
    {
        FVector CameraLocation = PlayerPawnCamera->GetRelativeLocation();

    	CameraLocation.Z += this->CameraZoomSpeed * this->CameraZoomAxisValue * DeltaSeconds;
        CameraLocation.Z = FMath::Clamp(CameraLocation.Z, this->MinCameraDistance, this->MaxCameraDistance);

    	PlayerPawnCamera->SetRelativeLocation(CameraLocation);
    }

	// Apply tilt-zoom input.
	if (TiltZoomValue != 0.0f)
	{
		this->OnApplyTiltZoom(TiltZoomValue);
	}

}

void APF2StrategicCameraPawnBase::FocusCameraOnActor(AActor* Actor)
{
	this->FocusCameraOnActors({ Actor });
}

void APF2StrategicCameraPawnBase::FocusCameraOnActors(TArray<AActor*> Actors)
{
	int32     ActorCount      = 0;
	FVector2D LocationSum     = FVector2D::ZeroVector;
	FVector2D CenterLocation;

	for (const AActor* Actor : Actors)
	{
		FVector ActorLocation;

		if (!IsValid(Actor))
		{
			continue;
		}

		ActorLocation = Actor->GetActorLocation();

		LocationSum.X += ActorLocation.X;
		LocationSum.Y += ActorLocation.Y;

		++ActorCount;
	}

	CenterLocation = LocationSum / ActorCount;

	this->FocusCameraOnLocation(CenterLocation);
}

void APF2StrategicCameraPawnBase::FocusCameraOnLocation(FVector2D NewCameraLocation)
{
    // Calculate where to put the camera, considering its angle, to center on the specified location.
    FVector FinalCameraLocation = FVector(NewCameraLocation.X - GetCameraDistance(), NewCameraLocation.Y, 0.0f);

    // Keep camera height.
    FinalCameraLocation.Z = this->GetActorLocation().Z;

    // Update camera location.
    this->SetActorLocation(FinalCameraLocation);
}

void APF2StrategicCameraPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveCameraRightLeft"), this, &APF2StrategicCameraPawnBase::MoveCameraRightLeft);
	PlayerInputComponent->BindAxis(TEXT("MoveCameraUpDown"),    this, &APF2StrategicCameraPawnBase::MoveCameraUpDown);
    PlayerInputComponent->BindAxis(TEXT("ZoomCamera"),          this, &APF2StrategicCameraPawnBase::ZoomCamera);
    PlayerInputComponent->BindAxis(TEXT("TiltZoomCamera"),      this, &APF2StrategicCameraPawnBase::TiltZoomCamera);
}

void APF2StrategicCameraPawnBase::MoveCameraRightLeft(const float Value)
{
    this->CameraRightLeftAxisValue = Value;
}

void APF2StrategicCameraPawnBase::MoveCameraUpDown(const float Value)
{
    this->CameraUpDownAxisValue = Value;
}

void APF2StrategicCameraPawnBase::ZoomCamera(const float Value)
{
    this->CameraZoomAxisValue = Value;
}

void APF2StrategicCameraPawnBase::TiltZoomCamera(const float Value)
{
	this->CameraTiltZoomAxisValue = Value;
}

float APF2StrategicCameraPawnBase::GetCameraDistance() const
{
	const UCameraComponent* Camera = this->FindComponentByClass<UCameraComponent>();
	float CameraAngle;

    if (!IsValid(Camera))
    {
        return 0.0f;
    }

    // Get camera angle.
    CameraAngle = Camera->GetRelativeRotation().Pitch;

    if (CameraAngle < 0.0f)
    {
        CameraAngle += 90.0f;
    }

    // Get camera distance using trigonometry.
    // We are assuming that the terrain is flat, centered at the origin, and the camera has no roll or yaw.
    return Camera->GetRelativeLocation().Z * FMath::Tan(FMath::DegreesToRadians(CameraAngle));
}
