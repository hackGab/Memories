#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonPersonnage.generated.h"

UCLASS()
class MEMORIESFPS_API AMonPersonnage : public ACharacter
{
	GENERATED_BODY()

public:
	AMonPersonnage();

	// Nécessaire pour enregistrer les variables à répliquer
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 1. VARIABLE RÉPLIQUÉE AVEC REPNOTIFY
	// Elle ne change que sur le serveur. Les clients exécutent OnRep_Sante quand ils reçoivent la mise à jour.
	UPROPERTY(ReplicatedUsing = OnRep_Sante, BlueprintReadOnly, Category = "Statistiques")
	float Sante;

	// Fonction appelée automatiquement sur le client lors de la réplication de "Sante"
	UFUNCTION()
	void OnRep_Sante();

	// Fonction locale pour modifier la santé (exécutée uniquement sur le serveur)
	void ModifierSante(float Montant);

	// 2. RPC : RUN ON SERVER
	// Appelée par le client, mais exécutée uniquement sur le serveur.
	// "WithValidation" permet de vérifier que le client ne triche pas.
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Action")
	void Server_DemanderDegats(float Degats);
}; 
