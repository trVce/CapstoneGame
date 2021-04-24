// Fill out your copyright notice in the Description page of Project Settings.
//Bradley Kisner: Module 2
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UCLASS()
class CAPSTONE2077_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
//The chunk of code below sets a ticker and sets two new camera variables that the developer will be able to assign cameras to in UE4. BK
public:	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	//Two new properties below will be used as actors to be assigned in the camera director within the Unreal Engine. BK
	UPROPERTY(EditAnywhere)
		AActor* CameraOne;

	UPROPERTY(EditAnywhere)
		AActor* CameraTwo;

	//Input Function
	void cameraGo();

	float TimeToNextCameraChange;
	bool cameraChange;

};
