#include "tkrzw_dbm_poly.h"
#include <napi.h>
#include "../include/config_parser.hpp"


class databaseWrapper : public Napi::ObjectWrap<databaseWrapper>
{
    private:
        tkrzw::PolyDBM dbm;
        std::map<std::string, std::string> optional_tuning_params;
    
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);          //required by Node!
        databaseWrapper(const Napi::CallbackInfo& info);
        Napi::Value set(const Napi::CallbackInfo& info);
        Napi::Value getSimple(const Napi::CallbackInfo& info);
        
};




//====================================================================================================================

databaseWrapper::databaseWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<databaseWrapper>(info)
{
    Napi::Env env = info.Env();
    Napi::String jsonConfigString = info[0].As<Napi::String>();
    std::string dbmPath = info[1].As<Napi::String>().ToString().Utf8Value();
    
    optional_tuning_params = parseConfig(env, jsonConfigString);
    tkrzw::Status opening_status = dbm.OpenAdvanced(dbmPath, true, tkrzw::File::OPEN_DEFAULT | tkrzw::File::OPEN_SYNC_HARD, optional_tuning_params).OrDie();
    if( opening_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, opening_status.GetMessage().c_str()).ThrowAsJavaScriptException();
    }
}

Napi::Value databaseWrapper::set(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();
    
    tkrzw::Status set_status = dbm.Set(key,value);
    if( set_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    else
    { return Napi::Boolean::New(env, true); }
}

Napi::Value databaseWrapper::getSimple(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string default_value = info[1].As<Napi::String>().ToString().Utf8Value();
    
    std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);
}

//====================================================================================================================

//-----------------JS-Requirements-----------------//
Napi::Object databaseWrapper::Init(Napi::Env env, Napi::Object exports)
{
    // This method is used to hook the accessor and method callbacks
    Napi::Function functionList = DefineClass(env, "databaseWrapper",
    {
        InstanceMethod<&databaseWrapper::set>("set", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&databaseWrapper::getSimple>("getSimple", static_cast<napi_property_attributes>(napi_writable | napi_configurable))
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(functionList);
    exports.Set("databaseWrapper", functionList);
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Object Init (Napi::Env env, Napi::Object exports)
{
    databaseWrapper::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
//-----------------JS-Requirements-----------------//