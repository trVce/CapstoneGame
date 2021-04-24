// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampFire.generated.h"

class UBoxComponent;
class UParticleSystemComponent;


UCLASS()
class CAPSTONE2077_API ACampFire : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACampFire();

public:	
	// declares particle system property fire. BK
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* Fire;
	//declares box component property myboxcomponent. BK
	UPROPERTY(EditAnywhere)
		UBoxComponent* MyBoxComponent;
	//declares subclass of a damage type called firedamage type. BK
	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> FireDamageType;
	//declares the actor property mycharacter. BK
	UPROPERTY(EditAnywhere)
		AActor* MyCharacter;
	//declares hitresult property myhit. BK
	UPROPERTY(EditAnywhere)
		FHitResult MyHit;
	// declares boolean variable to show whether or not damage can be applied. BK
	bool bCanApplyDamage;
	FTimerHandle FireTimerHandle;

	// declare component overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare component overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	//declares component apply fire damage function. BK
	UFUNCTION()
		void ApplyFireDamage();
};


