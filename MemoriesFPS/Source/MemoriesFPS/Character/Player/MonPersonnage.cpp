#include "MonPersonnage.h"
#include "Net/UnrealNetwork.h" // Obligatoire pour DOREPLIFETIME

AMonPersonnage::AMonPersonnage()
{
    // Indique à Unreal Engine que cet acteur gère le réseau
    bReplicates = true;
    
    Sante = 100.0f;
}

// RÈGLE OBLIGATOIRE : Enregistrement des variables réseau
void AMonPersonnage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Dit à Unreal de répliquer la variable Sante du Serveur vers TOUS les Clients
    DOREPLIFETIME(AMonPersonnage, Sante);
}

// Logique serveur standard
void AMonPersonnage::ModifierSante(float Montant)
{
    // Sécurité : Seul le serveur a l'autorité pour modifier la santé
    if (HasAuthority())
    {
        Sante = FMath::Clamp(Sante + Montant, 0.0f, 100.0f);
        
        // Note : Sur le serveur, OnRep n'est pas appelé automatiquement. 
        // On l'appelle manuellement si le serveur a aussi besoin de voir l'effet visuel.
        if (GetNetMode() == NM_ListenServer)
        {
            OnRep_Sante();
        }
    }
}

// Événement Client (RepNotify)
void AMonPersonnage::OnRep_Sante()
{
    // Cette logique s'exécute sur les machines des joueurs (Clients)
    // Idéal pour mettre à jour l'interface (HUD) ou jouer un son/effet de dégâts
    UE_LOG(LogTemp, Log, TEXT("Le client reçoit sa nouvelle santé : %f"), Sante);
}

// Implémentation du RPC Serveur (Générée automatiquement par Unreal avec le suffixe _Implementation)
void AMonPersonnage::Server_DemanderDegats_Implementation(float Degats)
{
    // Le serveur reçoit la demande du client et applique la logique de façon sécurisée
    ModifierSante(-Degats);
}

// Validation du RPC Serveur (Anti-triche, obligatoire avec WithValidation)
bool AMonPersonnage::Server_DemanderDegats_Validate(float Degats)
{
    // Exemple : On refuse la requête si la valeur de dégâts est impossible
    if (Degats < 0.0f || Degats > 100.0f)
    {
        return false; // Déconnecte le tricheur
    }
    return true;
}