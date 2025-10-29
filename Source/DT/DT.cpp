// Copyright Epic Games, Inc. All Rights Reserved.

#include "DT.h"
#include "Modules/ModuleManager.h"
#include "Tags/DTGameplayTags.h"

void FDTModule::StartupModule()
{
    // GameplayTag 초기화
    FDTGameplayTags::InitializeNativeTags();

    UE_LOG(LogTemp, Log, TEXT("Digital Twin Module Started"));
}

void FDTModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("Digital Twin Module Shutdown"));
}
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, DT, "DT" );
