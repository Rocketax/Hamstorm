// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "DHPlayerController.generated.h"

class ADHPipe;
class ADHDestructibleWall;
class UDHPlayerInputActionRegistry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWallDestroyed, AActor*, DestroyedWall, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryCharacterMove, AActor*, Tile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroyWall, AActor*, Wall);

//Class for the player controller. It constrols the camera directly (player pawn) and the hamster indirectly with the mouse clicks.
UCLASS(Blueprintable)
class HAMSTORM_API ADHPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void SetupInputComponent() override;

	UPROPERTY()
	FVector RemainingCameraMovement;

	UPROPERTY()
	bool IsHoldingRightMouse = false;

	UPROPERTY()
	bool IsHoldingMiddleMouse = false;

	UPROPERTY()
	AActor* HoveredActor;

public:
	//Input related variables.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inputs")
	UDHPlayerInputActionRegistry* InputActionRegistry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inputs")
	UInputMappingContext* InputMappingContext;

	//Gameplay related variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	AActor* TargetedDestructible;

	UPROPERTY(BlueprintAssignable, Category="Gameplay")
	FOnWallDestroyed OnWallDestroyed;

	UPROPERTY(BlueprintAssignable, Category="Gameplay")
	FOnTryCharacterMove OnTryCharacterMove;

	UPROPERTY(BlueprintAssignable, Category="Gameplay")
	FOnDestroyWall OnDestroyWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	bool IsInDialog = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	bool IsGamePaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	bool HasPickaxe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	bool HasHamsteritePickaxe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	bool HasPickedUpHamsteritePickaxe = false;

	//Camera related variables.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float InitialCameraRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraPanSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraZoomSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float CameraRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MaxZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float InitialZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float MinZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	float NoZoomDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool IsCameraRotating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool IsCameraFree = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool CanMoveCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool IsLastInputMouse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
	bool IsCameraRotationLocked = true;

	
	//Gameplay related functions.
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void ResetDestructible();

	UFUNCTION(BlueprintNativeEvent, Category="Gameplay")
	void OnAllPickaxesUsed();

	UFUNCTION(BlueprintNativeEvent, Category="Gameplay")
	void OnFirstHamsteritePickaxe();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void SetDestroyingADestructible(AActor* Wall);
	
	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void MoveCharacter(AActor* ClickedActor);

	//Camera related functions.
	UFUNCTION(BlueprintCallable, Category="Camera")
	void PanCamera(float DeltaX, float DeltaY);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetCameraRotation(float Angle, FRotator OldRotation);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnCameraRotate(float DeltaX);

	UFUNCTION(BlueprintNativeEvent, Category="Camera")
	void OnZoomUpdated(float Zoom);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void ClampCameraRotation();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void InitializeCamera();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void DisableFreeCamera();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void EnableFreeCamera();
	
	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetupCameraRotation(float DeltaX, float& Direction, FRotator& OldRotation);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void UpdateCameraRotation(float Alpha, float Direction, FRotator OldRotation, float Angle);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void FinishCameraRotation();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void AdjustCameraZoom(float Delta);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void SetCameraZoom(float Delta);
	

	//Input related functions.
	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnClickInput();

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnEnableCameraPanningInput();
	
	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnDisableCameraPanningInput();

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnEnableCameraRotationInput();
	
	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnDisableCameraRotationInput();
	
	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnMoveCameraInput(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnPanCameraKBInput(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnRotateCameraKBInput(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnAdjustCameraZoomInput(const FInputActionValue& ActionValue);

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnRecenterCameraInput();

	UFUNCTION(BlueprintCallable, Category="Inputs")
	void OnMouseMoved();

protected:
	virtual void BeginPlay() override;
};
