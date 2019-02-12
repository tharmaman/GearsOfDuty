// Fill out your copyright notice in the Description page of Project Settings.

#include <GearsOfDuty/Public/SCharacter.h>
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
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
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent -> BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
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

