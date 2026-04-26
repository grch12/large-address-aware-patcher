# large-address-aware-patcher

This program patches the "large address aware" flag in a 32-bit executable file. This flag, when set, allows the program to use memory addresses larger than 2GB.

## Usage

1. Download or compile the program.

2. Run the program by executing `./patcher` (for Linux, macOS, or Windows PowerShell) or `patcher.exe` (for Windows CMD).

3. Enter the path to the executable file you want to patch. If you drag a file into the terminal window, the path may be surrounded by quotes. If so, remove the quotes.

4. The program will back up the original file with a `.bak` extension.

5. If the backup cannot be created, the program will ask if you want to overwrite the original file.

6. The program will display "File patched successfully" if the operation is successful.

## Guides on Compiling

The precompiled binary is for x64 Windows and requires UCRT. If this is not the case, you can compile the source code yourself.

Since the program only uses the C standard library, it should compile on any platform with a compiler supporting C99. Simply clone this repository and run the command `gcc main.c -o patcher` in your terminal. However, note that the program requires a little-endian machine to run because PE files are little-endian.

## How It Works

See [my blog post](https://grch12.github.io/my-blog/posts/2026/large-address-aware.html) (in Chinese) for more details.

## License

This program is licensed under the MIT License. See the LICENSE file for more details.
