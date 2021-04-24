// Copyright Epic Games, Inc. All Rights Reserved.

#include "Capstone2077Character.h"
#include "Capstone2077Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Actor.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "CameraDirector.h"
#include "Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACapstone2077Character



ACapstone2077Character::ACapstone2077Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ACapstone2077Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Sets base health stats for the player below. BK
	FullHealth = 1000.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	PreviousHealth = HealthPercentage;
	bCanBeDamageChecker = true;
	//Sets base magic stats for the player below. BK
	KeyCount = 0;
	FullMagic = 100.0f;
	Magic = FullMagic;
	MagicPercentage = 1.0f;
	PreviousMagic = MagicPercentage;
	MagicValue = 0.0f;
	bCanUseMagic = true;
	//This if statement deals with the magic curve we created in UE4. Our magic states and values are binded to this curve, so they can update according the a change in the curve. A magic animation is also played here. BK.
	if (MagicCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("SetMagicState") });

		MyTimeline = NewObject<UTimelineComponent>(this, FName("Magic Animation"));
		MyTimeline->AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		MyTimeline->RegisterComponent();
	}

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

//This is a standard Tick method used for the UI. BK
void ACapstone2077Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyTimeline != nullptr) MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
}

void ACapstone2077Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire events StartFire and StopFire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACapstone2077Character::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACapstone2077Character::StopFire);

	//Bind Camera Event
	PlayerInputComponent->BindAction("CameraToggle", IE_Released, this, &ACapstone2077Character::OnCameraSwitch);

	// Bind DisplayRaycast event. BK
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this,
		&ACapstone2077Character::DisplayRaycast);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACapstone2077Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACapstone2077Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACapstone2077Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACapstone2077Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACapstone2077Character::LookUpAtRate);
}

//This method is responsible for the functionality of displaying the raycast in game. BK
void ACapstone2077Character::DisplayRaycast()
{	//On where the players crosshairs hit, the ray will be traced. BK
	FHitResult* Hitresult = new FHitResult();
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector EndTrace = ((ForwardVector * 3319.f) + StartTrace);
		
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*Hitresult, StartTrace, EndTrace,
		ECC_Visibility, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You hit: %s"),
			*Hitresult->Actor->GetName()));
	}
}

//This method utilizes FireShot and puts it into a loop that allows FireShot to be fully automatic.BK
void ACapstone2077Character::StartFire()
{
	FireShot();

	GetWorldTimerManager().SetTimer(TimerHandle_HandleRefire, this, &ACapstone2077Character::FireShot,TimeBetweenShots, true);
}

//This method stops the firing of the gun when the finger is off the left mouse key.BK
void ACapstone2077Character::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleRefire);
}

//This method is exactly the same as the  OnFire() method that used to exist in this cpp file.BK
void ACapstone2077Character::FireShot()
{	//This line was the only one changed in this method. This just states that if the player can use magic and a projectile exists, do the below funtions.BK
	if (ProjectileClass != NULL && !FMath::IsNearlyZero(Magic, 0.001f) && bCanUseMagic)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<ACapstone2077Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<ACapstone2077Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	if (MyTimeline != nullptr) MyTimeline->Stop();
	GetWorldTimerManager().ClearTimer(MagicTimerHandle);
	SetMagicChange(-20.0f);
	GetWorldTimerManager().SetTimer(MagicTimerHandle, this, &ACapstone2077Character::UpdateMagic, 5.0f, false);
}

//A getter for keycount
int ACapstone2077Character::GetKeyCount()
{
	return KeyCount;
}
//This is a simple health getter method. BK
float ACapstone2077Character::GetHealth()
{
	return HealthPercentage;
}
//This is a simple magic getter method. BK
float ACapstone2077Character::GetMagic()
{
	return MagicPercentage;
}

//THis method gets and prints text to correspond with the current health value the character has. BK
FText ACapstone2077Character::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}

//This method does the same thing as GetHealthIntText(), but with magic stats. BK
FText ACapstone2077Character::GetMagicIntText()
{
	int32 MP = FMath::RoundHalfFromZero(MagicPercentage * FullMagic);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString(TEXT("/")) + FullMPS;
	FText MPText = FText::FromString(MagicHUD);
	return MPText;
}

//This method sets the damage state of can be damaged to true so the character has the ability to take damage in game. BK
void ACapstone2077Character::SetDamageState()
{
	bCanBeDamageChecker = true;
}

//This method states the time intervals that the character can continuously take damage. BK
void ACapstone2077Character::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ACapstone2077Character::SetDamageState, 2.0f, false);
}

//This method sets the magic  statistic value for the player. BK
void ACapstone2077Character::SetMagicValue()
{
	TimelineValue = MyTimeline->GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimelineValue);
	Magic = CurveFloatValue * FullHealth;
	Magic = FMath::Clamp(Magic, 0.0f, FullMagic);
	MagicPercentage = CurveFloatValue;
	MagicPercentage = FMath::Clamp(MagicPercentage, 0.0f, 1.0f);
}

//This method sets the state of the magic for the player. BK
void ACapstone2077Character::SetMagicState()
{
	bCanUseMagic = true;
	MagicValue = 0.0;
	if (GunDefaultMaterial)
	{
		FP_Gun->SetMaterial(0, GunDefaultMaterial);
	}
}
//This boolean method decides whether or not to play a red flash when the player fires the gun. 
bool ACapstone2077Character::PlayFlash()
{
	if (redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}
//Updates the keycount positively or negatively.
void ACapstone2077Character::UpdateKeyCount(int keyNum)
{
	KeyCount += keyNum;

	if (KeyCount < 0) {
		KeyCount = 0;
	}
}

//This method updates the magic bar for the player after a shot fired. BK
void ACapstone2077Character::UpdateMagic()
{
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	if (MyTimeline != nullptr) MyTimeline->PlayFromStart();
}

//This method changes the magic value to whatever is left after a weapon fire.BK
void ACapstone2077Character::SetMagicChange(float MagicChange)
{
	bCanUseMagic = false;
	PreviousMagic = MagicPercentage;
	MagicValue = (MagicChange / FullMagic);
	if (GunOverheatMaterial)
	{
		FP_Gun->SetMaterial(0, GunOverheatMaterial);
	}

	if (MyTimeline != nullptr) MyTimeline->PlayFromStart();
}

//This method allows the player to actually take damage. BK
float ACapstone2077Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	bCanBeDamageChecker = false;
	redFlash = true;
	UpdateHealth(-DamageAmount);
	DamageTimer();
	return DamageAmount;
}

//This method updates the players health to its current value. BK
void ACapstone2077Character::UpdateHealth(float HealthChange)
{
	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
	PreviousHealth = HealthPercentage;
	HealthPercentage = Health / FullHealth;
}

void ACapstone2077Character::OnCameraSwitch() 
{
	cameraToggle();
}

void ACapstone2077Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACapstone2077Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		StartFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ACapstone2077Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ACapstone2077Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ACapstone2077Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACapstone2077Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACapstone2077Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACapstone2077Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
void ACapstone2077Character::cameraToggle() 
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed C button: Activated by Character Pawn."));
	ACameraDirector* CameraDirectorPointer = Cast<ACameraDirector>(CameraDirector);
	CameraDirectorPointer->cameraGo();
}

bool ACapstone2077Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACapstone2077Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ACapstone2077Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ACapstone2077Character::TouchUpdate);
		return true;
	}
	
	return false;
}
