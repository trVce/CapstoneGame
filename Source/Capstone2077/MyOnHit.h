// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyOnHit.generated.h"

UCLASS()
class CAPSTONE2077_API AMyOnHit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyOnHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//This UPROPERTY and UFUNCTION make sure that when a component in the game world is hit by the item that is attached to the onhitevent, the program will notify the user which surface is being hit. BK
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* MyComp;
	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
