// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "DHHamsterCharacter.generated.h"

class UDHHamsterPerceptionComponent;

//Struct for possible direction and length of movement to reach a tile.
USTRUCT(BlueprintType)
struct FDHDirectionLength
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Length;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPossibleTile, const TArray<FDHDirectionLength>&, PossibleDirection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTileChosen, AActor*, Destination, bool, IsAttacking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterMove, AActor*, Destination);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyVisionUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackRat, FVector, AttackDestination);

class UBoxComponent;

//Class for the hamster character.
UCLASS()
class HAMSTORM_API ADHHamsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADHHamsterCharacter();

	UPROPERTY()
	TArray<AActor*> Tiles;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="Movement")
	void CheckPossibleTiles();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Components")
	UStaticMeshComponent* PickaxeMesh;

	UPROPERTY(BlueprintAssignable, Category="Material")
	FOnPossibleTile OnPossibleTile;

	UPROPERTY(BlueprintAssignable, Category="Material")
	FOnTileChosen OnTileChosen;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Movement")
	FOnCharacterMove OnCharacterMove;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Movement")
	FOnEnemyVisionUpdated OnEnemyVisionUpdated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="Movement")
	FOnAttackRat OnAttackRat;

	UPROPERTY(BlueprintReadWrite, Category="Movement")
	TArray<AActor*> PossibleTiles;
	
	UPROPERTY(BlueprintReadWrite, Category="Movement")
	TArray<AActor*> PossibleDestroyableWalls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UBoxComponent* Center;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	AActor* CurrentTile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	bool CanMove = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	bool IsMoving = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	bool CanSeeEnemy = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	float RaycastDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	bool IsTileChosen;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement")
	AActor* LastSensedActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsDead = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsAttacking = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool HasUsedPipe = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Gameplay")
	bool IsFalling = false;
	
	TQueue<AActor*, EQueueMode::Spsc> QueuedMovement;

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddMoveAction();

	UFUNCTION(BlueprintCallable, Category="Action")
	void AddKillAction();

	UFUNCTION(BlueprintCallable, Category="Movement")
	bool IsRatInLoS();

	UFUNCTION(BlueprintCallable, Category="Movement")
	void StopMovement();

	UFUNCTION(BlueprintNativeEvent, Category="Movement")
	void WalkedIntoTrap();

	UFUNCTION(BlueprintNativeEvent, Category="Movement")
	void OnCloudNotTriggered();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Movement")
	void UpdatePickaxeMaterial(bool IsHamsterite);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void ResetEnemyVision();

	UFUNCTION(Blueprintable, Category="Movement")
	void ResetLoS();

	UFUNCTION(BlueprintCallable, Blueprintable, Category="Gameplay")
	void ResetQueuedMovement(AActor* Actor);
	
};
