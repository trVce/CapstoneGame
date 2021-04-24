// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Capstone2077Character.h"
#include "Capstone2077GameMode.generated.h"

//enum to store the current state of gameplay
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class ACapstone2077GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACapstone2077GameMode();
	/**Primary draw call for starting to play the game. BK*/
	virtual void BeginPlay() override;
	/**Primary draw call for the Tick method. BK*/
	virtual void Tick(float DeltaTime) override;

	ACapstone2077Character* MyCharacter;

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
		EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

private:
	/**Keeps track of the current playing state */
	EGamePlayState CurrentState;

	/**Handle any function calls that rely upon changing the playing state of our game */
	void HandleNewState(EGamePlayState NewState);
};




