import subprocess
import os.path

print(
    "Welcome to the ChihuahuaOS core setup utility! This will configure the project according to your " \
    "host environment.\n")

if not os.path.isfile("host_config.ini"):
    print(
        "You need a GCC cross-compiler to start. If you don't have one, look up how to build it on osdev.org. " \
        "It's also necessary to give the path to the directory containing the cross-compiler, as it needs to have " \
        "the following tools: '*-gcc', '*-g++', '*-ar', and '*-strip', where the wildcard will contain either " \
        "'x86_64-elf-' OR 'x86_64-w64-mingw32-', depending on the cross-compiler. If the PATH contains the directory, " \
        "you can just press 'Enter' when you are asked about the directory path.\n")

    print("The path to the directory containing the GCC cross-compiler (NOTE: use the absolute path, without any variables):", end='')
    gcc_cross_path = input()
    print(
        "The path to the GCC MinGW cross-compiler (x86_64-w64-mingw32-*, only for the bootloader) "
        "(NOTE: use the absolute path, without any variables):", 
        end='')
    gcc_uefi_cross_path = input()

    if gcc_cross_path != '' and not gcc_cross_path.endswith('/'):
        gcc_cross_path += '/'

    if gcc_uefi_cross_path != '' and not gcc_uefi_cross_path.endswith('/'):
        gcc_uefi_cross_path += '/'

    fs = open("host_config.ini", "w")
    fs.write(
        "[constants]\n" \
        f"gcc_cross_compiler_path = '{gcc_cross_path}'\n" \
        f"gcc_uefi_cross_compiler_path = '{gcc_uefi_cross_path}'\n"
    )
    fs.close()

    print("Paths written to 'host_config.ini'\n")
else:
    print("NOTE: if you want to reset the cross-compiler paths, delete the 'host_setup.ini' file.")

print(
    "You need to run 'meson setup' in all the projects with multiple '--cross-file' arguments and the correct " \
    "pkg-config prefixes.")

while True:
    print("Would you like to do run 'meson setup' now? [y/n]:", end='')
    should_run_setup = input().lower()

    if should_run_setup == 'y':
        subprocess.call(['bash', './config_meson_all.sh'])
        break
    elif should_run_setup == 'n':
        break
    else:
        print(f"Unknown option: {should_run_setup}. Please enter 'y' or 'n'.")

print("'meson setup' was ran, check its output to determine if it was successful or not.\n")
print("Configuration complete!")