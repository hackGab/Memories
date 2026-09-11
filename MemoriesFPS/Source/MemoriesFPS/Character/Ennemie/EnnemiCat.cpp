#include "EnnemiCat.h"
#include "../Player/MonPersonnage.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnnemiCat::AEnnemiCat()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnnemiCat::BeginPlay()
{
	Super::BeginPlay();

	Joueur = Cast<AMonPersonnage>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AEnnemiCat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Joueur) return;

	ComportementChat(DeltaTime);
}

void AEnnemiCat::ComportementChat(float DeltaTime)
{
	float Distance = FVector::Dist(GetActorLocation(), Joueur->GetActorLocation());

	// Le chat observe si le joueur est loin
	if (Distance > DistanceSuivi)
	{
		// Idle félin
		GetCharacterMovement()->StopMovementImmediately();
		return;
	}

	// Le chat suit le joueur
	if (Joueur->Sante > 70.0f)
	{
		FuirJoueur(DeltaTime);
		return;
	}

	if (Joueur->Sante < 40.0f)
	{
		// Mode agressif
		if (Distance < DistanceAttaque)
		{
			AttaquerJoueur();
		}
		else
		{
			SuivreJoueur(DeltaTime);
		}
		return;
	}

	// Comportement normal : suit doucement
	SuivreJoueur(DeltaTime);
}

void AEnnemiCat::SuivreJoueur(float DeltaTime)
{
	FVector Direction = (Joueur->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	AddMovementInput(Direction, 0.5f); // Marche lente comme un chat
}

void AEnnemiCat::FuirJoueur(float DeltaTime)
{
	FVector Direction = (GetActorLocation() - Joueur->GetActorLocation()).GetSafeNormal();
	AddMovementInput(Direction, 0.8f); // Fuite rapide
}

void AEnnemiCat::AttaquerJoueur()
{
	if (!Joueur) return;

	// Bond félin
	LaunchCharacter(GetActorForwardVector() * 400.0f + FVector(0,0,200), true, true);

	// Inflige des dégâts via ton RPC sécurisé
	Joueur->Server_DemanderDegats(DegatsChat);
}
