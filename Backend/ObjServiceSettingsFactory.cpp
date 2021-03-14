#include <ObjServiceSettingsFactory.h>

ObjServiceSettingsFactory::ObjServiceSettingsFactory()
{
    _settings = make_shared<Settings>();
    _settings->set_port(8081);
    _settings->set_default_header("Connection", "close");
    _settings->set_default_header("Access-Control-Allow-Origin", "*");
}

shared_ptr<Settings> ObjServiceSettingsFactory::get_settings() const
{
    return _settings;
}