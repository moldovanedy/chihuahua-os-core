//These are necessary for C++ to function correctly in a kernel environment,
//see https://wiki.osdev.org/C%2B%2B

/**
 * For pure virtual functions; should never get called
 */
extern "C" void __cxa_pure_virtual() // NOLINT(*-reserved-identifier)
{
    //TODO: print an error message.
}

/**
 * This is for static variable guards (specific to GCC).
 */
namespace __cxxabiv1 // NOLINT(*-reserved-identifier)
{
    /* guard variables */

    /* The ABI requires a 64-bit type.  */
    __extension__ typedef int __guard __attribute__((mode(__DI__))); // NOLINT(*-reserved-identifier)

    //TODO: we need to implement some mutexes here
    extern "C" int __cxa_guard_acquire (__guard *); // NOLINT(*-reserved-identifier)
    extern "C" void __cxa_guard_release (__guard *); // NOLINT(*-reserved-identifier)
    extern "C" void __cxa_guard_abort (__guard *); // NOLINT(*-reserved-identifier)

    extern "C" int __cxa_guard_acquire (__guard *g) 
    {
        return !*reinterpret_cast<char *>(g);
    }

    extern "C" void __cxa_guard_release (__guard *g)
    {
        *reinterpret_cast<char *>(g) = 1;
    }

    extern "C" void __cxa_guard_abort (__guard *)
    {

    }
}