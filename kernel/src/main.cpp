extern "C" [[noreturn]] void kernel_main(void) {
    while (true) {
#if __x86_64
        asm("cli");
        asm("hlt");
#endif
    }
}
