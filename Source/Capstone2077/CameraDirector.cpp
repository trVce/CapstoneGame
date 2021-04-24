// Fill out your copyright notice in the Description page of Project Settings.
//Bradley Kisner: Module 2

#include "CameraDirector.h"
#include "Capstone2077.h"
#include <Runtime\Engine\Classes\Kismet\GameplayStatics.h>

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned and set camera change to false so it doesn't automatically activate. BK
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();

    cameraChange = false;
	
}
void ACameraDirector::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
    //The lines below deal with the time between camera changes, and the change of the cameras themselves. BK
    //TimeBetweenCameraChanges is set to 3.0f so the camera will make the transition every 3 seconds. BK
    const float TimeBetweenCameraChange = 2.0f;
    //SmoothBlendTime is set to 0.75f so the time of the camera change will take 0.75 seconds to complete. BK
    const float SmoothBlendTime = 0.75f;
    TimeToNextCameraChange -= DeltaTime;

    if (cameraChange) 
    {
        if (TimeToNextCameraChange <= 0.0f) {
            TimeToNextCameraChange += TimeBetweenCameraChange;

            APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
            //If the player controller exists, and if CameraTwo exists and the player controller and view target is equal to CameraOne, the director will blend into CameraTwo.BK
            if (OurPlayerController) {
                if (CameraTwo && (OurPlayerController->GetViewTarget() == CameraOne))
                {
                    OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);

                }
                //If the above is not true, and if CameraOne exists, then the view target will be set to CameraOne.BK
                else if (CameraOne)
                {
                    OurPlayerController->SetViewTarget(CameraOne);
                }

            }
        }
        else {
            cameraChange = false;
            TimeToNextCameraChange = 0.0f;
        }
    }
}
// Called every frame
/**void ACameraDirector::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //The lines below deal with the time between camera changes, and the change of the cameras themselves. BK
    //TimeBetweenCameraChanges is set to 3.0f so the camera will make the transition every 3 seconds. BK
    const float TimeBetweenCameraChanges = 3.0f;
    //SmoothBlendTime is set to 0.75f so the time of the camera change will take 0.75 seconds to complete. BK
    const float SmoothBlendTime = 0.75f;
    TimeToNextCameraChange -= DeltaTime;
    if (TimeToNextCameraChange <= 0.0f)
    {
        TimeToNextCameraChange += TimeBetweenCameraChanges;
        APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
        //If the player controller exists, and if CameraTwo exists and the player controller and view target is equal to CameraOne, the director will blend into CameraTwo.BK
        if (OurPlayerController) {
            if (CameraTwo && (OurPlayerController->GetViewTarget() == CameraOne))
            {
                OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);

            }
            //If the above is not true, and if CameraOne exists, then the view target will be set to CameraOne.BK
            else if (CameraOne)
            {
                OurPlayerController->SetViewTarget(CameraOne);
            }

        }

    }
}*/

//This method makes sure sets the camera change to true so that the camera director can make the change on key input. BK
void ACameraDirector::cameraGo() 
{
    cameraChange = true;
    TimeToNextCameraChange = 0.0f;

    UE_LOG(LogTemp, Warning, TEXT("cameraGo Activated!"));
}