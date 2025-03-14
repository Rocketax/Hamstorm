// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/DHPickaxe.h"
#include "DHHamsteritePickaxe.generated.h"

//Class for hamsterite pickaxe (used to destroy pillars).
UCLASS()
class HAMSTORM_API ADHHamsteritePickaxe : public ADHPickaxe
{
	GENERATED_BODY()

	virtual bool TryPickup() override;
};
