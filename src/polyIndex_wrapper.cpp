#include "../include/polyIndex_wrapper.hpp"

polyIndex_wrapper::polyIndex_wrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<polyIndex_wrapper>(info)
{
    Napi::Env env = info.Env();
    std::map<std::string, std::string> optional_tuning_params;
    if(info[0].IsString())
    {
        Napi::String jsonConfigString = info[0].As<Napi::String>();
        optional_tuning_params = parseConfig(env, jsonConfigString);
    }
    else if(info[0].IsObject())
    {
        Napi::Object configObject = info[0].As<Napi::Object>();
        optional_tuning_params = parseConfig(env, configObject);
    }
    std::string indexPath = info[1].As<Napi::String>().ToString().Utf8Value();

    tkrzw::Status opening_status = index.Open(indexPath, true, tkrzw::File::OPEN_DEFAULT | tkrzw::File::OPEN_SYNC_HARD, optional_tuning_params).OrDie();
    if( opening_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, opening_status.GetMessage().c_str()).ThrowAsJavaScriptException();
    }
}

Napi::Value polyIndex_wrapper::add(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_ADD, key, value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::getValues(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    size_t max_number_of_records = info[1].As<Napi::Number>().Int64Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_GET_VALUES, key, max_number_of_records);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::check(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_CHECK, key, value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::remove(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_REMOVE, key, value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::shouldBeRebuilt(const Napi::CallbackInfo& info)
{
    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_SHOULD_BE_REBUILT);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::rebuild(const Napi::CallbackInfo& info)
{
    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), index, dbmAsyncWorker::INDEX_REBUILD);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyIndex_wrapper::close(const Napi::CallbackInfo& info)
{
    std::cout << "CLOSE INDEX" << std::endl;
    Napi::Env env = info.Env();
    tkrzw::Status close_status = index.Close();
    if( close_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, close_status.GetMessage().c_str()).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    else
    { return Napi::Boolean::New(env, true); }
}

//====================================================================================================================

//-----------------JS-Requirements-----------------//
Napi::Object polyIndex_wrapper::Init(Napi::Env env, Napi::Object exports)
{
    // This method is used to hook the accessor and method callbacks
    Napi::Function functionList = DefineClass(env, "polyIndex",
    {
        InstanceMethod<&polyIndex_wrapper::add>("add", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::getValues>("getValues", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::check>("check", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::remove>("remove", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::shouldBeRebuilt>("shouldBeRebuilt", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::rebuild>("rebuild", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyIndex_wrapper::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable))
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(functionList);
    exports.Set("polyIndex", functionList);
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

void polyIndex_wrapper::Finalize(Napi::BasicEnv env)
{
    tkrzw::Status close_status = index.Close();
    if( close_status != tkrzw::Status::SUCCESS)
    {
        std::cerr << "Index finalize: Failed!" << std::endl;
    }
    else
    { std::cerr << "Index finalize: SUCCESS!" << std::endl; }   
}
//-----------------JS-Requirements-----------------//