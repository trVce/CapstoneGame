// Fill out your copyright notice in the Description page of Project Settings.


#include "CampFire.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACampFire::ACampFire()
{
    MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
    MyBoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    RootComponent = MyBoxComponent;

    Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
    Fire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Fire->SetupAttachment(RootComponent);

    MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACampFire::OnOverlapBegin);
    MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACampFire::OnOverlapEnd);

    bCanApplyDamage = false;

}

//This method states that when the player overlaps the camp fire, damage will be dealt to the player over a period of time. BK
void ACampFire::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        bCanApplyDamage = true;
        MyCharacter = Cast<AActor>(OtherActor);
        MyHit = SweepResult;
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);
    }
}

//This method states that when the player walks away from the fire, the damage will stop and the player will not get hurt anymore. BK
void ACampFire::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    bCanApplyDamage = false;
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

//This method applies the fire damage that is used in the method above. BK.
void ACampFire::ApplyFireDamage()
{
    if (bCanApplyDamage)
    {
        UGameplayStatics::ApplyPointDamage(MyCharacter, 200.0f, GetActorLocation(), MyHit, nullptr, this, FireDamageType);
    }
}




