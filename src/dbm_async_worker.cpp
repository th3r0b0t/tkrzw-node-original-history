#include "../include/dbm_async_worker.hpp"

dbmAsyncWorker::dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, std::string param1, std::string param2):
Napi::AsyncWorker(env),
env(env),
dbmReference(dbmReference),
operation(operation),
param1(param1),
param2(param2),
deferred_promise{env}
{};

void dbmAsyncWorker::Execute()
{
    if(operation == SET)
    {
        tkrzw::Status set_status = dbmReference.Set(param1,param2);
        if( set_status != tkrzw::Status::SUCCESS)
        {
            //Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
            SetError("Couldn't set!");
        }
    }
    else if(operation == GET_SIMPLE)
    {
        result = dbmReference.GetSimple(param1,param2);
        if(result == param2)
        {
            SetError("Key \"" + param1 + "\" not found!");
        }
    }
}

void dbmAsyncWorker::OnOK()
{
    deferred_promise.Resolve(Napi::String::New(env, result));
}

void dbmAsyncWorker::OnError(const Napi::Error& err)
{
    deferred_promise.Reject(err.Value());
}