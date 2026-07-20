// By Gabriel Laplante on 2026-07-20


#include "MemoriesGameState.h"
#include "Net/UnrealNetwork.h"


void AMemoriesGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMemoriesGameState, TimeRemaining);
}