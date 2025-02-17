// Copyright Epic Games, Inc. All Rights Reserved.

#include "Brackeys_Jam_2025Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Rewind/RewindComponent.h"
#include "Rewind/RewindGameMode.h"
#include "Rewind/RewindVisualizationComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABrackeys_Jam_2025Character

ABrackeys_Jam_2025Character::ABrackeys_Jam_2025Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Setup a rewind component that snapshots 30 times per second
	RewindComponent = CreateDefaultSubobject<URewindComponent>(TEXT("RewindComponent"));
	RewindComponent->SnapshotFrequencySeconds = 1.0f / 30.0f;
	RewindComponent->bSnapshotMovementVelocityAndMode = true;
	RewindComponent->bPauseAnimationDuringTimeScrubbing = true;

	// Setup a rewind visualization component that draws a static mesh instance for each snapshot
	RewindVisualizationComponent = CreateDefaultSubobject<URewindVisualizationComponent>(TEXT("RewindVisualizationComponent"));
	RewindVisualizationComponent->SetupAttachment(RootComponent);
}

void ABrackeys_Jam_2025Character::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Capture game mode for driving global rewind
	GameMode = Cast<ARewindGameMode>(GetWorld()->GetAuthGameMode());

	// Capture original camera arm length to restore after rewinding
	OriginalTargetCameraArmLength = CameraBoom->TargetArmLength;

	// Bind to time manipulation events
	RewindComponent->OnTimeManipulationStarted.AddUniqueDynamic(this, &ABrackeys_Jam_2025Character::UpdateCamera);
	RewindComponent->OnTimeManipulationCompleted.AddUniqueDynamic(this, &ABrackeys_Jam_2025Character::UpdateCamera);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABrackeys_Jam_2025Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ABrackeys_Jam_2025Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABrackeys_Jam_2025Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABrackeys_Jam_2025Character::Look);
		// Toggle Time Scrub
		EnhancedInputComponent->BindAction(ToggleTimeScrubAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::ToggleTimeScrub);

		// Rewind
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::Rewind);
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Completed, this, &ABrackeys_Jam_2025Character::StopRewinding);

		// Fast Forward
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::FastForward);
		EnhancedInputComponent->BindAction(FastForwardAction, ETriggerEvent::Completed, this, &ABrackeys_Jam_2025Character::StopFastForwarding);

		// Set Rewind Speed
		EnhancedInputComponent->BindAction(
			SetRewindSpeedSlowestAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::SetRewindSpeedSlowest);
		EnhancedInputComponent->BindAction(
			SetRewindSpeedSlowerAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::SetRewindSpeedSlower);
		EnhancedInputComponent->BindAction(
			SetRewindSpeedNormalAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::SetRewindSpeedNormal);
		EnhancedInputComponent->BindAction(
			SetRewindSpeedFasterAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::SetRewindSpeedFaster);
		EnhancedInputComponent->BindAction(
			SetRewindSpeedFastestAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::SetRewindSpeedFastest);

		// Toggle Rewind Participation
		EnhancedInputComponent->BindAction(
			ToggleRewindPartipationAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::ToggleRewindParticipation);

		// Toggle Timeline Splines
		EnhancedInputComponent->BindAction(
			ToggleTimelineVisualizationAction, ETriggerEvent::Started, this, &ABrackeys_Jam_2025Character::ToggleTimelineVisualization);
	}
	else
	{
		UE_LOG(
			LogTemplateCharacter,
			Error,
			TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend "
				"to use the legacy system, then you will need to update this C++ file."),
			*GetNameSafe(this));
	}
}

void ABrackeys_Jam_2025Character::Jump()
{
	// Ignore input while manipulating time
	if (RewindComponent->IsTimeBeingManipulated()) { return; }

	Super::Jump();
}


void ABrackeys_Jam_2025Character::StopJumping()
{
	// Ignore input while manipulating time
	if (RewindComponent->IsTimeBeingManipulated()) { return; }

	Super::StopJumping();
}


void ABrackeys_Jam_2025Character::Move(const FInputActionValue& Value)
{
	// Ignore input while manipulating time
	if (RewindComponent->IsTimeBeingManipulated()) { return; }

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABrackeys_Jam_2025Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Add rotation directly to the camera boom if manipulating time
	if (RewindComponent->IsTimeBeingManipulated())
	{
		// Clamp pitch to avoid awkward camera rotation above the player character
		FRotator NewRotation = CameraBoom->GetRelativeRotation();
		NewRotation.Yaw += LookAxisVector.X;
		NewRotation.Pitch -= LookAxisVector.Y;
		NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -80.0f, 80.0f);
		CameraBoom->SetRelativeRotation(NewRotation);
	}
	else if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABrackeys_Jam_2025Character::ToggleTimeScrub(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->ToggleTimeScrub(); }
}

void ABrackeys_Jam_2025Character::Rewind(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->StartGlobalRewind(); }
}

void ABrackeys_Jam_2025Character::StopRewinding(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->StopGlobalRewind(); }
}

void ABrackeys_Jam_2025Character::FastForward(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->StartGlobalFastForward(); }
}

void ABrackeys_Jam_2025Character::StopFastForwarding(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->StopGlobalFastForward(); }
}

void ABrackeys_Jam_2025Character::SetRewindSpeedSlowest(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->SetRewindSpeedSlowest(); }
}

void ABrackeys_Jam_2025Character::SetRewindSpeedSlower(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->SetRewindSpeedSlow(); }
}

void ABrackeys_Jam_2025Character::SetRewindSpeedNormal(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->SetRewindSpeedNormal(); }
}

void ABrackeys_Jam_2025Character::SetRewindSpeedFaster(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->SetRewindSpeedFast(); }
}

void ABrackeys_Jam_2025Character::SetRewindSpeedFastest(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->SetRewindSpeedFastest(); }
}

void ABrackeys_Jam_2025Character::ToggleRewindParticipation(const FInputActionValue& Value)
{
	RewindComponent->SetIsRewindingEnabled(!RewindComponent->IsRewindingEnabled());
}

void ABrackeys_Jam_2025Character::ToggleTimelineVisualization(const FInputActionValue& Value)
{
	check(GameMode);
	if (GameMode) { GameMode->ToggleGlobalTimelineVisualization(); }
}

void ABrackeys_Jam_2025Character::UpdateCamera()
{
	if (RewindComponent->IsTimeBeingManipulated())
	{
		// Switch to bird's eye camera view
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->TargetArmLength = RewindTargetCameraArmLength;
	}
	else
	{
		// Restore original camera view
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->TargetArmLength = OriginalTargetCameraArmLength;
	}
}
