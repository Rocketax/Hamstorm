// Fill out your copyright notice in the Description page of Project Settings.


#include "Environment/DHEndPipe.h"

#include "Components/DecalComponent.h"

ADHEndPipe::ADHEndPipe()
{
	PrimaryActorTick.bCanEverTick = false;

	//Setting up components
	Pipe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pipe"));
	SetRootComponent(Pipe);
	
	Entrance = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Entrance"));
	Entrance->SetupAttachment(RootComponent);
	
	Sign = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sign"));
	Sign->SetupAttachment(Entrance);
	
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(Sign);
}
