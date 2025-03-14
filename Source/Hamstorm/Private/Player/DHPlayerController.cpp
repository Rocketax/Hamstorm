// Fill out your copyright notice in the Description page of Project Settings.


#include "Hamstorm/Public/Player/DHPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Environment/DHDestructibleWall.h"
#include "Hamster/DHHamsterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\Public\Player\DHPlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Environment/DHDestructiblePillar.h"
#include "Environment/DHEndPipe.h"
#include "Environment/DHFloorTile.h"
#include "Environment/DHPipe.h"
#include "Game/DHGameState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/Inputs/DHPlayerInputActionRegistry.h"

//This functions sets up all the input mappings with their respective function.
void ADHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

#pragma region ENHANCED_INPUTS_MAPPING

	const auto* LocalPlayer = GetLocalPlayer();

	check(LocalPlayer);

	auto* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	check(EnhancedInputLocalPlayerSubsystem);

	EnhancedInputLocalPlayerSubsystem->ClearAllMappings();

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputMappingContext, 0);

#pragma endregion

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

	check(EnhancedInput);

	check(InputActionRegistry);

	EnhancedInput->BindAction(InputActionRegistry->OnClickInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnClickInput);

	EnhancedInput->BindAction(InputActionRegistry->OnEnableCameraPanningInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnEnableCameraPanningInput);

	EnhancedInput->BindAction(InputActionRegistry->OnDisableCameraPanningInput, ETriggerEvent::Triggered, this,
	
	                          &ADHPlayerController::OnDisableCameraPanningInput);

	EnhancedInput->BindAction(InputActionRegistry->OnEnableCameraRotationInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnEnableCameraRotationInput);

	EnhancedInput->BindAction(InputActionRegistry->OnDisableCameraRotationInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnDisableCameraRotationInput);

	EnhancedInput->BindAction(InputActionRegistry->OnMoveCameraInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnMoveCameraInput);

	EnhancedInput->BindAction(InputActionRegistry->OnAdjustCameraZoomInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnAdjustCameraZoomInput);

	EnhancedInput->BindAction(InputActionRegistry->OnPanCameraKBInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnPanCameraKBInput);

	EnhancedInput->BindAction(InputActionRegistry->OnRotateCameraKBInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnRotateCameraKBInput);

	EnhancedInput->BindAction(InputActionRegistry->OnRecenterCameraInput, ETriggerEvent::Triggered, this,
	                          &ADHPlayerController::OnRecenterCameraInput);
}

//This function is called multiple times to smooth out the camera rotation.
void ADHPlayerController::UpdateCameraRotation(float Alpha, float Direction, FRotator OldRotation, float Angle = 90)
{
	float AngleToRotate = FMath::Lerp(0, Direction * Angle, Alpha);

	SetCameraRotation(AngleToRotate, OldRotation);
}

//This function cleans up after the camera rotation is finished.
void ADHPlayerController::FinishCameraRotation()
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	PlayerPawn->SpringArm->bEnableCameraLag = true;

	IsCameraRotating = false;
}

void ADHPlayerController::OnClickInput()
{
	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));

	FHitResult HitResult;

	GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	ADHGameState* GameState = Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()));

	// Exit conditions. We don't consider the click in all of these situations.
	if (IsInDialog || IsGamePaused || HamsterCharacter->IsMoving || HamsterCharacter->IsDead || HamsterCharacter->IsAttacking
		|| GameState->IsHamstormDoneMoving || GameState->IsDestroyingADestructible) return;
	if (!IsValid(HitResult.GetActor())) return;
	// End exit conditions

	//The player clicked on a pipe.
	if (HitResult.GetActor()->IsA(ADHPipe::StaticClass()))
	{
		ADHPipe* Pipe = Cast<ADHPipe>(HitResult.GetActor());

		//Hamstorm isnt on the tile in front of the pipe's entrance.
		if (!Pipe->CanUsePipe)
		{
			TArray<AActor*> Overlapping;

			Pipe->BoxComponent->GetOverlappingActors(Overlapping, ADHFloorTile::StaticClass());

			//We queue a movement to get him there and use it if the tile in front of the pipe is accessible.
			if  (HamsterCharacter->PossibleTiles.Contains(Overlapping[0]))
			{
				Pipe->AutoTeleport = true;

				Pipe->ShouldResetAutoTeleport = true;
				
				MoveCharacter(Overlapping[0]);
			}
		}
		//Hamstorm is in front of the pipe and can use it.
		else
		{
			Pipe->UsePipe();

			DisableFreeCamera();

			Pipe->CanUsePipe = false;
		}
	}
	//The player clicked on the pipe that ends the level.
	else if(HitResult.GetActor()->IsA(ADHEndPipe::StaticClass()))
	{
		ADHEndPipe* Pipe = Cast<ADHEndPipe>(HitResult.GetActor());
		
		if  (HamsterCharacter->PossibleTiles.Contains(Pipe->EndTile))
		{
			MoveCharacter(Pipe->EndTile);
		}
	}
	//The player clicked on a destructible wall or a destructible tile.
	else if (HitResult.GetActor()->IsA(ADHDestructibleWall::StaticClass())
		|| (HitResult.GetActor()->IsA(ADHFloorTile::StaticClass()) && Cast<ADHFloorTile>(HitResult.GetActor())->Destructible))
	{
		//We don't consider the click if the player doesn't have the appropriate pickaxe for the destructible.
		if (HitResult.GetActor()->IsA(ADHDestructiblePillar::StaticClass()) && !HasHamsteritePickaxe) return;
		if (!HasPickaxe && !HitResult.GetActor()->IsA(ADHDestructiblePillar::StaticClass())) return;
		
		IDHDestructibleInterface* Destructible = Cast<IDHDestructibleInterface>(HitResult.GetActor());

		if (ADHFloorTile* AdjacentTile = Destructible->CheckValidAdjacentTile())
		{
			Destructible->SetShouldBeDestroyed(true);

			TargetedDestructible = Cast<AActor>(Destructible);

			//Hamstorm is on the adjacent tile
			if (AdjacentTile == HamsterCharacter->CurrentTile) MoveCharacter(HitResult.GetActor());
			//He isn't. We queue a movement to get him there.
			else MoveCharacter(AdjacentTile);
		}
	}
	//The player clicked on a normal tile.
	else MoveCharacter(HitResult.GetActor());
}

//The following two functions handle the locking and unlocking of the camera panning input for the mouse.
void ADHPlayerController::OnEnableCameraPanningInput()
{
	if (IsGamePaused) return;
	IsHoldingRightMouse = true;
}

void ADHPlayerController::OnDisableCameraPanningInput()
{
	if (IsGamePaused) return;
	IsHoldingRightMouse = false;
}

//The following two functions handle the locking and unlocking of the camera rotation input for the mouse.
void ADHPlayerController::OnEnableCameraRotationInput()
{
	if (IsGamePaused) return;
	IsHoldingMiddleMouse = true;
}

void ADHPlayerController::OnDisableCameraRotationInput()
{
	if (IsGamePaused) return;
	IsHoldingMiddleMouse = false;
}

//This function moves and rotates the camera when the player is holding the correct buttons.
void ADHPlayerController::OnMoveCameraInput(const FInputActionValue& ActionValue)
{
	if (IsGamePaused) return;
	
	FVector2d Delta = ActionValue.Get<FVector2d>();

	//Camera panning with mouse
	if (IsHoldingRightMouse)
	{
		IsLastInputMouse = true;
		
		PanCamera(Delta.X * 1.5, Delta.Y * 1.5);
	}
	//Camera rotation with mouse
	else if (IsHoldingMiddleMouse && !IsCameraRotating)
	{
		IsLastInputMouse = true;
		
		OnCameraRotate(Delta.X);
	}
}

//This function handles the panning of the camera with the keyboard.
void ADHPlayerController::OnPanCameraKBInput(const FInputActionValue& ActionValue)
{
	if (IsGamePaused) return;
	
	FVector2d Delta = ActionValue.Get<FVector2d>();

	IsLastInputMouse = false;

	PanCamera(Delta.X, Delta.Y);
}

//This function handles the rotation of the camera with the keyboard.
void ADHPlayerController::OnRotateCameraKBInput(const FInputActionValue& ActionValue)
{
	//Camera is already rotating or the game is paused.
	if (IsCameraRotating || IsGamePaused) return;

	float Delta = ActionValue.Get<float>();

	IsLastInputMouse = false;

	OnCameraRotate(Delta);
}

//This function handles the zoom of the camera.
void ADHPlayerController::OnAdjustCameraZoomInput(const FInputActionValue& ActionValue)
{
	if (IsGamePaused) return;
	
	const float MouseWheelDelta = ActionValue.Get<float>();

	AdjustCameraZoom(MouseWheelDelta);
}

//This function recenters the camera on the hamster.
void ADHPlayerController::OnRecenterCameraInput()
{
	if (IsGamePaused) return;
	
	DisableFreeCamera();
}

//This function is used to highlight the different interactible objects to give a visual feedback.
void ADHPlayerController::OnMouseMoved()
{
	//We ignore mouse movements if the player is trying to control the camera.
	if (IsHoldingRightMouse || IsHoldingMiddleMouse) return;

	FHitResult HitResult;

	GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	//The mouse hasn't moved from the previous hovered actor.
	if (HitResult.GetActor() == HoveredActor) return;

	//The mouse is no longer on the previous hovered actor. We stop highlighting it.
	if (HoveredActor)
	{
		IDHHoverableInterface::Execute_OnStopHovered(HoveredActor);

		HoveredActor = nullptr;
	}

	//We are hovering on an actor.
	if (HitResult.GetActor())
	{
		//We are hovering on an interactible object. We start highlighting it.
		if (HitResult.GetActor()->GetClass()->ImplementsInterface(UDHHoverableInterface::StaticClass()))
		{
			IDHHoverableInterface* Hoverable = Cast<IDHHoverableInterface>(HitResult.GetActor());

			if (Hoverable->IsHovered) return;

			IDHHoverableInterface::Execute_OnStartHovered(HitResult.GetActor());

			HoveredActor = HitResult.GetActor();
		}
	}
}

void ADHPlayerController::OnAllPickaxesUsed_Implementation()
{
	
}

void ADHPlayerController::OnFirstHamsteritePickaxe_Implementation()
{
	
}

//This funtion sets a variable that indicates that the player is currently destroying a destructible.
void ADHPlayerController::SetDestroyingADestructible(AActor* Wall)
{
	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));
	
	if (HamsterCharacter->IsDead) return;
	
	Cast<ADHGameState>(UGameplayStatics::GetGameState(GetWorld()))->IsDestroyingADestructible = true;
}

void ADHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Setting up the camera.
	InitializeCamera();

	if (!IsCameraFree)
	{
		DisableFreeCamera();
	}

	//Binding wall destruction event.
	OnDestroyWall.AddDynamic(this, &ADHPlayerController::SetDestroyingADestructible);
}

//This function is used when reverting to make sure the hamster doesn't go back to the destructible after reverting.
void ADHPlayerController::ResetDestructible()
{
	if (TargetedDestructible)
	{
		Cast<IDHDestructibleInterface>(TargetedDestructible)->SetShouldBeDestroyed(false);
	}
}

//This function handles the movement of the hamster.
void ADHPlayerController::MoveCharacter(AActor* ClickedActor)
{
	AActor* HamsterActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass());

	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(HamsterActor);

	if (!HamsterCharacter->CanMove) return;

	//The actor that was clicked was a destructible wall or pillar.
	if (ClickedActor->IsA(ADHDestructibleWall::StaticClass()))
	{
		//The destructible is accessible by the hamster.
		if (HamsterCharacter->PossibleDestroyableWalls.Contains(ClickedActor))
		{
			//We destroy it if we have the matching pickaxe.
			if ((ClickedActor->IsA(ADHDestructiblePillar::StaticClass()) && HasHamsteritePickaxe)
				|| (!ClickedActor->IsA(ADHDestructiblePillar::StaticClass()) && HasPickaxe))
			{
				OnDestroyWall.Broadcast(ClickedActor);
			}
		}
	}
	//The actor that was clicked was a floor tile.
	else if (ClickedActor->IsA(ADHFloorTile::StaticClass()))
	{
		auto Tile = Cast<ADHFloorTile>(ClickedActor);

		//The tile is destructible, it isn't weakened, it is accessible by the hamster and the player has the right pickaxe.
		//We destroy it.
		if (Tile->Destructible && HasPickaxe && !Tile->Destroyed && HamsterCharacter->PossibleDestroyableWalls.
			Contains(ClickedActor))
		{
			OnDestroyWall.Broadcast(Tile);
		}
		//There was no tile clicked before.
		else if (!HamsterCharacter->IsTileChosen)
		{
			OnTryCharacterMove.Broadcast(ClickedActor);
		}
	}
	//The actor that was clicked was an interactible object.
	else if (ClickedActor->GetClass()->ImplementsInterface(UDHHoverableInterface::StaticClass()))
	{
		TArray<AActor*> OverlappingActors;
		
		ClickedActor->GetOverlappingActors(OverlappingActors);
		
		//We move towards the overlapping tile.
		for (auto Actor : OverlappingActors)
		{
			UE_LOG(LogTemp, Warning, TEXT("Overlapping actor: %s"), *Actor->GetName())
			if (Actor->IsA(ADHFloorTile::StaticClass()))OnTryCharacterMove.Broadcast(Actor);
		}
	}
}

//This function handles the panning of the camera with both the keyboard and the mouse.
void ADHPlayerController::PanCamera(float DeltaX, float DeltaY)
{
	if (!CanMoveCamera) return;
	
	if (!IsCameraFree)
	{
		EnableFreeCamera();
	}

	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	FVector DirectionX = PlayerPawn->SpringArm->GetRightVector() * -DeltaX;

	FVector DirectionY = PlayerPawn->SpringArm->GetUpVector() * -DeltaY;

	FVector WorldDirection = DirectionX + DirectionY;

	WorldDirection.Z = 0;

	PlayerPawn->AddActorWorldOffset(WorldDirection * CameraPanSpeed * GetWorld()->GetDeltaSeconds(), true);
}

//This function sets the camera rotation instantly without smoothing.
void ADHPlayerController::SetCameraRotation(float Angle, FRotator OldRotation)
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	PlayerPawn->SpringArm->SetWorldRotation(OldRotation + FRotator(0, Angle, 0));
}

void ADHPlayerController::OnCameraRotate_Implementation(float DeltaX)
{
}

void ADHPlayerController::OnZoomUpdated_Implementation(float Zoom)
{
	
}

//This function makes sure that the camera's rotation stays at the target angles. We aim for an isometric view.
void ADHPlayerController::ClampCameraRotation()
{
	TArray TargetAngles = {45.f, 135.f, 225.f, 315.f};
	
	const ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	const float Rotation = PlayerPawn->SpringArm->GetComponentRotation().Yaw;

	const float AbsRotation = Rotation >= 0 ? Rotation : 360 - FMath::Abs(Rotation);

	//We find the closest angle to snap to.
	float ClosestAngle = TargetAngles[0];

	float MinAngleDif = FMath::Abs(TargetAngles[0] - AbsRotation);

	for (const float Angle : TargetAngles)
	{
		if (FMath::Abs(Angle - AbsRotation) < MinAngleDif)
		{
			MinAngleDif = FMath::Abs(Angle - AbsRotation);

			ClosestAngle = Angle;
		}
	}
	//

	FRotator OldRotation = PlayerPawn->SpringArm->GetComponentRotation();

	OldRotation.Yaw = 0;
	
	SetCameraRotation(ClosestAngle, OldRotation);
}

//This function sets up the camera at the start of the game.
void ADHPlayerController::InitializeCamera()
{
	float Direction;
	FRotator OldRotation;

	SetupCameraRotation(1, Direction, OldRotation);

	SetCameraRotation(InitialCameraRotation, OldRotation);

	SetCameraZoom(InitialZoom);

	FinishCameraRotation();
}

//This function locks the camera onto the hamster.
void ADHPlayerController::DisableFreeCamera()
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	ADHHamsterCharacter* HamsterCharacter = Cast<ADHHamsterCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ADHHamsterCharacter::StaticClass()));

	PlayerPawn->AttachToActor(HamsterCharacter, FAttachmentTransformRules::KeepWorldTransform);

	PlayerPawn->SetActorLocation(HamsterCharacter->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);

	FRotator LookAtPointRotator = UKismetMathLibrary::FindLookAtRotation(
		PlayerPawn->GetActorLocation(), HamsterCharacter->GetActorLocation());

	LookAtPointRotator.Pitch = 0;

	PlayerPawn->SetActorRotation(LookAtPointRotator);

	ClampCameraRotation();

	IsCameraFree = false;
}

//This function allows the player to move the camera freely.
void ADHPlayerController::EnableFreeCamera()
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	PlayerPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	IsCameraFree = true;
}

//This function sets up variables for the camera rotation at the start of the game.
void ADHPlayerController::SetupCameraRotation(float DeltaX, float& Direction, FRotator& OldRotation)
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	PlayerPawn->SpringArm->bEnableCameraLag = false;

	IsCameraRotating = true;

	Direction = DeltaX < 0 ? -1 : 1;

	OldRotation = PlayerPawn->SpringArm->GetComponentRotation();
}

//This function adjusts the zoom by increasing or decreasing the spring arm length.
void ADHPlayerController::AdjustCameraZoom(float Delta)
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	const float Distance = PlayerPawn->SpringArm->TargetArmLength - Delta * CameraZoomSpeed;

	PlayerPawn->SpringArm->TargetArmLength = FMath::Clamp(
		Distance,
		NoZoomDistance / MaxZoom,
		NoZoomDistance / MinZoom
	);

	const float Zoom = NoZoomDistance / PlayerPawn->SpringArm->TargetArmLength;

	OnZoomUpdated(Zoom);
}

//This function instantly sets the zoom.
void ADHPlayerController::SetCameraZoom(float Zoom)
{
	ADHPlayerPawn* PlayerPawn = Cast<ADHPlayerPawn>(GetPawn());

	Zoom = FMath::Clamp(Zoom, MinZoom, MaxZoom);

	PlayerPawn->SpringArm->TargetArmLength = NoZoomDistance / Zoom;
}
