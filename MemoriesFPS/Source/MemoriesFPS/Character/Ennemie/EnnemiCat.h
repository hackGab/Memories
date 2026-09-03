#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnnemiCat.generated.h"

class AMonPersonnage;

UCLASS()
class MEMORIESFPS_API AEnnemiCat : public ACharacter
{
	GENERATED_BODY()

public:
	AEnnemiCat();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	AMonPersonnage* Joueur;

	UPROPERTY(EditAnywhere, Category="Chat|Comportement")
	float DistanceSuivi = 600.0f;

	UPROPERTY(EditAnywhere, Category="Chat|Comportement")
	float DistanceAttaque = 150.0f;

	UPROPERTY(EditAnywhere, Category="Chat|Comportement")
	float DegatsChat = 10.0f;

	void ComportementChat(float DeltaTime);
	void AttaquerJoueur();
	void SuivreJoueur(float DeltaTime);
	void FuirJoueur(float DeltaTime);
};
