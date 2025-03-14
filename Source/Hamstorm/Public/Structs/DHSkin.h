// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "DHSkin.generated.h"

//This struct is used in a data table to store all the different skins of Hamstorm. There is no logic in this struct.
USTRUCT(BlueprintType)
struct HAMSTORM_API FDHSkin
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skin")
	USkeletalMesh* SkinMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skin")
	UNiagaraSystem* SkinAura;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skin")
	TArray<UMaterialInterface*> SkinMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skin")
	FName SkinName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skin")
	bool IsUnlocked;
	
	FDHSkin();
	
	~FDHSkin();
};
