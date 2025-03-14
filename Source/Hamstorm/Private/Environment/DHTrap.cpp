// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHTrap.h"

ADHTrap::ADHTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up trap mesh component.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}
