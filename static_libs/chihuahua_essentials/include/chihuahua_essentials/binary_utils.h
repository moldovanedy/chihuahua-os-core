#ifndef CHIHUAHUA_ESSENTIALS_BINARY_UTILS_H
#define CHIHUAHUA_ESSENTIALS_BINARY_UTILS_H

namespace BinaryUtils {
/**
 * Sets the "index" bit (from the least significant bit) of "target" to 1.
 * @param target
 * @param index
 */
#define ESSENTIALS_SET_BIT_ON(target, index) ((target) |= 1 << (index))

/**
 * Sets the "index" bit (from the least significant bit) of "target" to 0.
 * @param target
 * @param index
 */
#define ESSENTIALS_SET_BIT_OFF(target, index) ((target) &= ~(1 << (index)))

/**
 * Sets the "bits" with the "index" offset (from the least significant bit) of "target" to the given "bits" value.
 * The bits are additively added (with an "OR", so if the value is 1 and the "bits" bit is 0, it will still remain 1).
 * @param target
 * @param bits
 * @param index
 */
#define ESSENTIALS_SET_BITS_ADDITIVE(target, bits, index) ((target) |= (bits) << (index))

#define ESSENTIALS_CHECK_BIT(target, index) (((target) & (1 << (index))) == index)
    
} //namespace BinaryUtils

#endif //CHIHUAHUA_ESSENTIALS_BINARY_UTILS_H