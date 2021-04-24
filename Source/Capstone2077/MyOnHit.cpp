// Fill out your copyright notice in the Description page of Project Settings.


#include "MyOnHit.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyOnHit::AMyOnHit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//MyComp is set to a new subobject UBoxComponent that has physics implemented on it along with notifications for collision on surfaces in game. BK
	MyComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	MyComp->SetSimulatePhysics(true);
	MyComp->SetNotifyRigidBodyCollision(true);

	MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	MyComp->OnComponentHit.AddDynamic(this, &AMyOnHit::OnCompHit);

	// Set as root component
	RootComponent = MyComp;

}

// Called when the game starts or when spawned
void AMyOnHit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyOnHit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//This method add the on screen messages that pop up when the component that is created hits a surface in the game. BK
void AMyOnHit::OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *OtherActor->GetName()));
	}
}

