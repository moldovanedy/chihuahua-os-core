#include "chihuahua_essentials/result.h"

namespace Utils {
    template<class T, class E>
    Result<T, E>::Result(const T& success) {
        this->success = success;
    }

    template<class T, class E>
    Result<T, E>::Result(const E& error) {
        this->error = error;
    }

    template<class T, class E>
    void Result<T, E>::match(const SuccessFunctor onSuccess, const ErrorFunctor onError) {
        if (isSuccessful()) {
            onSuccess(getSuccessUnchecked());
        }
        else {
            if (!error.hasValue()) {
                //ERROR, but we can't really do anything here... this should be impossible
            }

            onError(getErrorUnchecked());
        }
    }
} //namespace Utils