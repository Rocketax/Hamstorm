// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "DHPlayerInputActionRegistry.generated.h"

//Class for all input actions. No logic here.
UCLASS(Blueprintable)
class HAMSTORM_API UDHPlayerInputActionRegistry : public UInputAction
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnClickInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnEnableCameraPanningInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnDisableCameraPanningInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnEnableCameraRotationInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnDisableCameraRotationInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnPanCameraKBInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnRotateCameraKBInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnMoveCameraInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnAdjustCameraZoomInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inputs")
	UInputAction* OnRecenterCameraInput;
};
