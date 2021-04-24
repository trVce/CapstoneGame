// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MedKit.generated.h"

UCLASS()
class CAPSTONE2077_API AMedKit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMedKit();


public:	
	//declares an on overlap function so the player can later pick up the medkit on overlap. BK
	UFUNCTION()
		void OnOverlap(AActor * MyOverlappedActor, AActor * OtherActor);

};
