// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorKey.h"
#include "Capstone2077Character.h"
// Sets default values
ADoorKey::ADoorKey()
{
	OnActorBeginOverlap.AddDynamic(this, &ADoorKey::OnOverlap);

}

//states that on overlap with the key, the players keycount will increase
void ADoorKey::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		class ACapstone2077Character* MyCharacter = Cast<ACapstone2077Character>(OtherActor);

		if (MyCharacter)
		{
			MyCharacter->UpdateKeyCount(1);
			Destroy();
		}
	}
}
