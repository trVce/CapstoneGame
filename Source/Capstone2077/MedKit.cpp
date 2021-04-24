// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKit.h"
#include "Capstone2077Character.h"


// Sets default values
AMedKit::AMedKit()
{
    OnActorBeginOverlap.AddDynamic(this, &AMedKit::OnOverlap);

}

//states that on overlap with the medkit, the players health will increase if the player has lost health.BK
void AMedKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		class ACapstone2077Character* MyCharacter = Cast<ACapstone2077Character>(OtherActor);

		if (MyCharacter && MyCharacter->GetHealth() < 1.0f)
		{
			MyCharacter->UpdateHealth(100.0f);
			Destroy();
		}
	}
}

