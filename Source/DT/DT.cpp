// Copyright Epic Games, Inc. All Rights Reserved.

#include "DT.h"
#include "Modules/ModuleManager.h"
#include "Tags/DTGameplayTags.h"

class FDTGameModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override
    {
        FDefaultGameModuleImpl::StartupModule();
        FDTGameplayTags::InitializeNativeTags();
    }
};
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, DT, "DT" );
