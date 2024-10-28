#include "tkrzw_dbm_poly.h"
#include <napi.h>

class dbmAsyncWorker : public Napi::AsyncWorker
{
    public:
        enum OPERATION_TYPE
        {
            SET,
            GET_SIMPLE 
        };

        dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, std::string param1, std::string param2);

        void Execute() override;
        void OnOK() override;
        void OnError(const Napi::Error& err) override;
        
        Napi::Promise::Deferred deferred_promise;

    private:
        std::string param1;
        std::string param2;
        std::string result;
        OPERATION_TYPE operation;
        tkrzw::PolyDBM& dbmReference;
        const Napi::Env& env;
};