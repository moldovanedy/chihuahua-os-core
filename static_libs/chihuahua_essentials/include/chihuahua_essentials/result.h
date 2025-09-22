#ifndef CHIHUAHUA_ESSENTIALS_RESULT_H
#define CHIHUAHUA_ESSENTIALS_RESULT_H

#include "option.h"

namespace Utils {
    template <class T, class E>
    class Result {
        Option<T> success;
        Option<E> error;

    public:
        typedef void (*SuccessFunctor)(T success);
        typedef void (*ErrorFunctor)(E error);

        explicit Result(const T& success);
        explicit Result(const E& error);

        bool isSuccessful() {
            return success.hasValue();
        }

        T getSuccessUnchecked() {
            return success.getValue();
        }

        E getErrorUnchecked() {
            return error.getValue();
        }

        void match(SuccessFunctor onSuccess, ErrorFunctor onError);
    };
} //namespace Utils

#endif //CHIHUAHUA_ESSENTIALS_RESULT_H