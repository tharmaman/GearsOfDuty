// Fill out your copyright notice in the Description page of Project Settings.

#include <GearsOfDuty/Public/SCharacter.h>
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "SCharacter.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp -> bUsePawnControlRotation = true;
	SpringArmComp -> SetupAttachment(RootComponent);

	// enabling support for crouching
	GetMovementComponent() -> GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp -> SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp -> FieldOfView;

	// spawn a default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// spawn a default weapon
	CurrentWeapon = GetWorld() -> SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon -> SetOwner(this);
		CurrentWeapon -> AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp -> FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp -> SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent -> BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent -> BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	// two ways of doing the same thing
	PlayerInputComponent -> BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	(*PlayerInputComponent).BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent -> BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent -> BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	// jump code
	PlayerInputComponent -> BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

    // for zooming
    PlayerInputComponent -> BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
    PlayerInputComponent -> BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent -> BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent -> BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
    bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
    bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if(CurrentWeapon)
	{
		CurrentWeapon -> StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon -> StopFire();
	}
}

FVector ASCharacter::GetPawnViewLocation() const
{
	// don't need to inherit from super if camera exists
	if (CameraComp)
	{
		return CameraComp -> GetComponentLocation();
	}

	// otherwise return super
	return Super::GetPawnViewLocation();
}

