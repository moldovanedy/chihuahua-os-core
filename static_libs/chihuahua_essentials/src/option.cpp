#include "chihuahua_essentials/option.h"

namespace Utils {
    template<class T>
    Option<T>::Option(const T &value) : doesHaveValue(true), value(value) {}

    template<class T>
    Option<T>::Option(T value, bool) : doesHaveValue(true), value(value) {}

    template<class T>
    Option<T>::Option() : doesHaveValue(false) {
    }


} //namespace Utils