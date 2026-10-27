# Some ELF relocation things for purplek2

I'm just trying to understand ELF loading and relocation for my kernel project, [purplek2](https://github.com/purpleK2/kernel/tree/meson-rewrite). I'll leave this repository here just in case someone wants to learn too :)

# Building
This is yet another Meson project, you know the drill. You only need a C compiler installed, no external libraries.
```sh
meson setup build
meson compile -C build
```

# Usage
The program expects an ELF file (to be parsed) as a parameter:
```sh
./build/libelf-pk2 path/to/elf
```

# Notes?

If you just want to fix the warnings or refine the the code overall, then contributions are welcome.
