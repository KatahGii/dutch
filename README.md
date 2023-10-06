
# Dutch

## 📄 Description
"Dutch" is a utility program designed to update the access and modification times of files. It serves as an almost drop-in replacement for the Unix `touch` command, offering additional features.

## ✨ Features
- **Timestamp Updates**: Refresh file timestamps to the current time.
- **Custom Timestamps**: Specify a particular timestamp for updates.
- **Reference File**: Use another file as a reference for timestamp updates.
- **Symbolic Links**: Option to not dereference symbolic links.
- **Silent Mode**: Suppress error messages.
- **No-Create Mode**: Skip creating new files if they don't exist.

## 🔧 Usage
```bash
Usage: dutch [OPTION] FILE...
Update the access and modification times of each FILE to the current time.

Type 'dutch --help' to see detailed usage information.
```

## 📜 Options
(Note: This is a summarized list; the actual program may offer more detailed options.)
- `-a`: Change only the access time.
- `-m`: Change only the modification time.
- `-c`: Avoid creating the file if it doesn't exist.
- `-d`: Use the specified timestamp.
- `-t`: Another option to use a specific timestamp.
- `-r`: Use the timestamp of a reference file.
- `-h`: Do not dereference symbolic links.
- `-s`: Operate silently.

## 📌 Version
`dutch version 1.0.0`

## 🖋 Author
`Copyright (C) 2023 Katahgii`

## 🛠 Installation and Compilation
To compile the `dutch` program, use a C compiler like `gcc`:
```bash
gcc -o dutch dutch.c
```
This command will produce an executable named `dutch`.

If you want to use it by just typing in `dutch` instead of `./dutch` then use this command:
```bash
mv dutch /usr/local/bin/
```

## 📜 License
Please refer to the source code or accompanying documentation for licensing information.
