// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DHHoverableInterface.h"
#include "DHPipe.generated.h"

class ADHGameState;
class ADHPlayerController;
class ADHHamsterCharacter;
class UArrowComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartTeleportation);

UCLASS()
class HAMSTORM_API ADHPipe : public AActor, public IDHHoverableInterface
{
	GENERATED_BODY()

	UPROPERTY()
	ADHPlayerController* PlayerController;
	
	UPROPERTY()
	ADHHamsterCharacter* HamsterCharacter;
	
	UPROPERTY()
	ADHGameState* GameState;
	
public:	
	// Sets default values for this actor's properties
	ADHPipe();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	ADHPipe* OtherSide;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	UBoxComponent* BoxComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	UArrowComponent* ArrowComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	USceneComponent* TeleportingPoint;

	UPROPERTY(BlueprintReadWrite, Category="Pipe")
	bool CanUsePipe = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	bool AutoTeleport = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	bool ShouldRotateCamera = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	bool ShouldResetAutoTeleport = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Pipe")
	float OutCameraAngle;

	UPROPERTY(BlueprintAssignable, EditAnywhere, Category="Pipe")
	FOnStartTeleportation OnStartTeleportation;

	UFUNCTION(Blueprintable, Category="Pipe")
	void UsePipe();

	UFUNCTION(Blueprintable, Category="Action")
	void AddTeleportAction(AActor* Hamster);

	UFUNCTION(BlueprintCallable, Category="Pipe")
	void Teleport();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
						bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
