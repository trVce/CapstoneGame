// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorKey.generated.h"

UCLASS()
class CAPSTONE2077_API ADoorKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorKey();

public:	
	UFUNCTION()
		void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);

};
