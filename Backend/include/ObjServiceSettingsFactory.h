#pragma once

#include <IServiceSettingsFactory.h>

class ObjServiceSettingsFactory : public IServiceSettingsFactory
{
private:
    shared_ptr<Settings> _settings;

public:
    ObjServiceSettingsFactory();
    shared_ptr<Settings> get_settings() const final;
};
