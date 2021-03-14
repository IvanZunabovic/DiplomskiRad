#include "ObjResourceFactory.h"
#include "ObjServiceSettingsFactory.h"
#include "ObjService.h"

int main(int argc, char **argv)
{
    auto resource_factory = make_shared<ObjResourceFactory>();
    auto settings_factory = make_shared<ObjServiceSettingsFactory>();
    ObjService service{resource_factory, settings_factory};

    service.start();

    return 0;
}