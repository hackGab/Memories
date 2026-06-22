# Memories

## À propos

**Memories** est un jeu escape game à la première personne développé avec Unreal Engine.  
Ce dépôt contient les fichiers du projet, les ressources du jeu et le journal de développement.

---

# Journal de développement

## 9 juin 2026

### Progression

- Création de la base du projet FPS sous Unreal Engine.
- Mise en place et ajustement des collisions des escaliers afin d'assurer un déplacement fluide du joueur.
- Création et intégration d'une texture/matériau de miroir pour les surfaces réfléchissantes.

### Prochaines étapes

- Implémenter le système d'interaction des portes :
  - Ouvrir les portes
  - Fermer les portes
- Développer les premiers objets interactifs de l'environnement.
- Mettre en place un système d'interaction réutilisable pour les futures mécaniques de jeu.

### Notes

Cette première étape de développement avait pour objectif de mettre en place les fondations du projet et de s'assurer que les interactions de base avec l'environnement fonctionnent correctement. La prochaine phase sera consacrée aux systèmes d'interaction du joueur et à l'amélioration de l'immersion.

---

## 22 juin 2026

### Progression

- Implémentation d’un système de **ladder (échelle)** permettant au joueur de monter et descendre de façon fluide.
- Ajout d’une mécanique de **sprint** pour améliorer la mobilité du joueur en exploration.
- Développement et intégration d’un système d’interaction amélioré pour les objets manipulables (prise en main, rotation, lancement).

### Système d’interaction – Variables utilisées

| Variable | Type | Purpose |
| --- | --- | --- |
| **IsHeld** | Bool | Indique si l’objet est actuellement inspecté / tenu par le joueur |
| **HoldDistance** | Float | Distance à laquelle l’objet flotte devant la caméra |
| **RotationSpeed** | Float | Vitesse de rotation de l’objet avec la souris |
| **ThrowStrength** | Float | Force appliquée lors du lancer de l’objet |
| **OriginalSimulatePhysics** | Bool | Permet de restaurer la physique après avoir relâché l’objet |
| **HeldByCharacter** | Character Ref | Référence au personnage pour accéder à la caméra et à la direction de lancer |
| **HoldOffset** | Vector | Ajustement fin de la position de l’objet pendant l’interaction |

### Notes

Cette mise à jour marque une avancée importante dans la fluidité du gameplay. Le joueur dispose maintenant de plus de contrôle sur sa mobilité (sprint + ladder) ainsi que d’un système d’interaction plus stable et extensible pour les mécaniques futures.
