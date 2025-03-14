// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DHGameState.generated.h"

class ADHPlayerController;
class ADHHamsterCharacter;
class UDHTurn;
class ADHRat;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTurnEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHamsterDied, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRatMovementEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRevertEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarDestroyed, FText, ZoneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRatInvasionReverted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZoneChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHamsterDeathFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFirstPickaxeUse);

//Class for gamestate. Handles a lot of the game logic such as turns, events, reverts and some links to the HUD.
//It also stores multiple variables that represent the state of the game.
UCLASS()
class HAMSTORM_API ADHGameState : public AGameStateBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<AActor*> MovingRats;

	UPROPERTY()
	TArray<AActor*> Rats;

	UPROPERTY()
	ADHHamsterCharacter* HamsterCharacter;

	UPROPERTY()
	ADHPlayerController* PlayerController;

public:

	UPROPERTY(BlueprintReadWrite, Category="Turns")
	TArray<UDHTurn*> Turns;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assets")
	TArray<UStaticMesh*> FloorAssets;

	UPROPERTY(BlueprintAssignable, Category="Gameplay")
	FOnTurnEnded OnTurnEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnRatMovementEnded OnRatMovementEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnMovementEnded OnMovementEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnHamsterDied OnHamsterDied;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnRevertEnded OnRevertEnded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnPillarDestroyed OnPillarDestroyed;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnRatInvasionReverted OnRatInvasionReverted;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnZoneChanged OnZoneChanged;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnHamsterDeathFinished OnHamsterDeathFinished;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Gameplay")
	FOnFirstPickaxeUse OnFirstPickaxeUse;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	int32 CurrentLevel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	int32 NbMovingRats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	int32 NbRatsMoved;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	FString CurrentZone = "HubZone";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsTutorialCompleted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsFirstTimeEnteringFinalZone = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool CanRevertTurn = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsReverting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool HasReverted = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsQueuedMovement = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsTileTriggered = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsHamstormDoneMoving = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool HasAlreadyAddedTurn = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsDestroyingADestructible = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	TArray<AActor*> DestroyedPillars;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Statistics")
	int32 NbRatsKilled;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Statistics")
	int32 NbTurns;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Statistics")
	int32 NbTurnsAtCheckpoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsCollectibleCollected = false;

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void TryEndTurn();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void RevertTurn();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void CheckEndTurn();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void UpdateNbRatsMoved();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void ResetAttackingRats();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	bool AreEnemiesAttacking();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	bool AreEnemiesDying();

	UFUNCTION(BlueprintNativeEvent, Category="Gameplay")
	void OnFirstTimeEnteringFinalZone();

	UFUNCTION()
	void ResetRevertVariables();

	UFUNCTION(BlueprintCallable, Category="Checkpoint")
	void SetCheckpoint();

	UFUNCTION(BlueprintCallable, Category="Checkpoint")
	void RestartFromCheckpoint();

	UFUNCTION(BlueprintCallable, Category="Spawn")
	void AddRatInvasionAction(TArray<AActor*> InvadingRats);

	UFUNCTION()
	void ResetRatArrays();
};
