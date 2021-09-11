// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EScreenDividerMovementStyle : uint8
{
	Linear,
	Smooth,
	MoreSmooth,
	SmoothStart,
	SmoothEnd,
	Overshoot
};
