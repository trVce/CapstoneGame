// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Capstone2077HUD.generated.h"

UCLASS()
class ACapstone2077HUD : public AHUD
{
	GENERATED_BODY()

public:
	ACapstone2077HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	/**Primary draw call for starting to play the game. BK*/
	virtual void BeginPlay() override;


private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	//The two properties below are Health Widgets. BK
	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget* CurrentWidget;

};

