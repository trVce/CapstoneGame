// Copyright Epic Games, Inc. All Rights Reserved.

#include "Capstone2077HUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ACapstone2077HUD::ACapstone2077HUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
	//Finds and utilizes the Health_UI UI component we created in UE4. BK
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Health_UI"));
	HUDWidgetClass = HealthBarObj.Class;
}


void ACapstone2077HUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X),
		(Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}
	//This BeginPlay() method sets all the widgets to be ready to use in the HUD when the game starts. BK
	void ACapstone2077HUD::BeginPlay()
	{
		Super::BeginPlay();

		if (HUDWidgetClass != nullptr)
		{
			CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport();
			}
		}
	}
