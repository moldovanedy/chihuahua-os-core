#ifndef CHIHUAHUA_ESSENTIALS_OPTION_H
#define CHIHUAHUA_ESSENTIALS_OPTION_H

namespace Utils {
    template <class T>
    class Option {
        bool doesHaveValue;
        T value;

    public:
        explicit Option(const T &value);

        /**
         * Returns an option by copying the value. The second parameter is just for disambiguation.
         * @param value 
         */
        explicit Option(T value, bool);
        explicit Option();

        [[nodiscard]]
        bool hasValue() const {
            return doesHaveValue;
        }

        T getValue() const {
            return value;
        }
    };
} //namespace Utils
#endif //CHIHUAHUA_ESSENTIALS_OPTION_H