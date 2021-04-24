// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSwitchTrigger.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
// include draw debu helpers header file
#include "DrawDebugHelpers.h"

// Sets default values
ALightSwitchTrigger::ALightSwitchTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//This sets the intensity of the light of the actor. Raising the value below will brighten and widen the range of light within its area.BK
	LightIntensity = 3000.0f;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->Intensity = LightIntensity;
	//Found this fix to be usable in 4.25.4. If you replace bVisible with bAddedToSceneVisible below, the light should appear within its bubble in the game world. BK
	PointLight->bAddedToSceneVisible = true;
	RootComponent = PointLight;
	//The code below sets the area that the light will be centered within. Once the character collides with the sphere, the light should turn off. BK
	LightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Light Sphere Component"));
	LightSphere->InitSphereRadius(300.0f);
	LightSphere->SetCollisionProfileName(TEXT("Trigger"));
	LightSphere->SetupAttachment(RootComponent);

	LightSphere->OnComponentBeginOverlap.AddDynamic(this, &ALightSwitchTrigger::OnOverlapBegin);
	LightSphere->OnComponentEndOverlap.AddDynamic(this, &ALightSwitchTrigger::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ALightSwitchTrigger::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugSphere(GetWorld(), GetActorLocation(), 300.f, 50, FColor::Green, true, -1, 0, 2);
	
}

// Called every frame
void ALightSwitchTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//This method toggles the light to turn off when the character overlaps with the light bubble.BK
void ALightSwitchTrigger::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ToggleLight();
	}
}
//This method toggles the light back on when the character leaves the light bubble.BK
void ALightSwitchTrigger::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ToggleLight();
	}
}

//This method toggles the visibility of the light source in its bubble. BK
void ALightSwitchTrigger::ToggleLight()
{
	PointLight->ToggleVisibility();
}

