#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DHHamsterController.generated.h"

class UDHHamsterPerceptionComponent;

//Class for the AI Controller of the hamster.
UCLASS()
class HAMSTORM_API ADHHamsterController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> Pickaxes;

	UPROPERTY()
	TArray<AActor*> Tiles;

	UPROPERTY()
	TArray<AActor*> Rats;

public:
	ADHHamsterController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;

	UFUNCTION(BlueprintCallable, Category="AI")
	void SetDestination(AActor* Destination);

	UFUNCTION(BlueprintCallable, Category="AI")
	void ValidateTile(AActor* Destination);

protected:
	virtual void BeginPlay() override;
};