#pragma once

#include <IResourceFactory.h>
#include <tuple>
#include <string>

class ObjResourceFactory : public IResourceFactory
{
public:
    ObjResourceFactory();
    shared_ptr<Resource> get_resource() const final;

private:
    shared_ptr<Resource> _resource;
    tuple<string> get_path_parameters(const shared_ptr<Session> session);
    void get_handler(const shared_ptr<Session> session);
    float load_object(string object);
    string to_json(float result);
};