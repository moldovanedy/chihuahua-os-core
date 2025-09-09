print(
    "Welcome to the ChihuahuaOS core setup utility! This will configure the project according to your " \
    "host environment.\n")
print(
    "You need a GCC cross-compiler to start. If you don't have one, look up how to build it on osdev.org. " \
    "It's also necessary to give the path to the directory containing the cross-compiler, as it needs to have " \
    "the following tools: '*-gcc', '*-g++', '*-ar', and '*-strip', where the wildcard will contain either " \
    "'x86_64-elf-' OR 'x86_64-w64-mingw32-', depending on the cross-compiler. If the PATH contains the directory, " \
    "you can just press 'Enter' when you are asked about the directory path.\n")

print("The path to the directory containing the GCC cross-compiler:", end='')
gcc_cross_path = input()
print("The path to the GCC MinGW cross-compiler (x86_64-w64-mingw32-*, only for the bootloader):", end='')
gcc_uefi_cross_path = input()

fs = open("host_config.ini", "w")
fs.write(
    "[constants]\n" \
    f"gcc_cross_compiler_path = '{gcc_cross_path}'\n" \
    f"gcc_uefi_cross_compiler_path = '{gcc_uefi_cross_path}'\n"
)
fs.close()

print("Paths written to 'host_config.ini'\n")

print(
    "You need to run 'meson setup' in all the projects with multiple '--cross-file' arguments and the correct " \
    "pkg-config prefixes.")
print("Would you like to do it now? [y/n]:", end='')
should_run_setup = input()