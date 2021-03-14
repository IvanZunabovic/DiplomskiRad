#include <ObjResourceFactory.h>

#include <sstream>
#include <iomanip>

#include "json.hpp"

ObjResourceFactory::ObjResourceFactory()
{
    _resource = make_shared<Resource>();
    _resource->set_path("/{getObject: getObject}");
    _resource->set_method_handler("GET", [&](const shared_ptr<Session> session) {
        get_handler(session);
    });
}

shared_ptr<Resource> ObjResourceFactory::get_resource() const
{
    return _resource;
}

void ObjResourceFactory::get_handler(const shared_ptr<Session> session)
{
    const auto [object] = get_path_parameters(session);
    const auto result = load_object(object);
    auto content = to_json(result);

    session->close(OK, content, {{"Content-Lengt", to_string(content.size())}});
}

tuple<string> ObjResourceFactory::get_path_parameters(const shared_ptr<Session> session)
{
    const auto &request = session->get_request();
    auto object = request->get_path_parameter("getObject");

    return make_tuple(object);
}

float ObjResourceFactory::load_object(string object)
{
    return 1.0;
}

string ObjResourceFactory::to_json(float result)
{
    ostringstream str_stream;
    str_stream << result;

    nlohmann::json jsonResult = {{"result", str_stream.str()}};

    return jsonResult.dump();
}