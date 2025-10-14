


#include "FactoryObjectAttributes.h"
#include "Net/UnrealNetwork.h"

UFactoryObjectAttributes::UFactoryObjectAttributes()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitOperationalStatus(1.0f); // 1.0 = On
}

void UFactoryObjectAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UFactoryObjectAttributes, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFactoryObjectAttributes, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFactoryObjectAttributes, OperationalStatus, COND_None, REPNOTIFY_Always);
}

void UFactoryObjectAttributes::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFactoryObjectAttributes, Health, OldHealth);
}

void UFactoryObjectAttributes::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFactoryObjectAttributes, MaxHealth, OldMaxHealth);
}

void UFactoryObjectAttributes::OnRep_OperationalStatus(const FGameplayAttributeData& OldOperationalStatus)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFactoryObjectAttributes, OperationalStatus, OldOperationalStatus);
}