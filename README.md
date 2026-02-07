# licgen - Offline ChooseALicense CLI

`licgen` is a command-line tool that brings the functionality of [choosealicense.com](https://choosealicense.com/) to your terminal, completely offline. It helps you quickly generate license files for your projects without having to search for them online.

## Features

- **Offline License Help**: View permissions, conditions, and limitations for popular licenses in a clean table format.
- **Dynamic Template System**: Adding new licenses is as easy as creating a `.template` text file.
- **Smart Defaults**: Automatically writes to `LICENSE.txt`, `LICENSE.md`, or `LICENSE.json` based on the format.
- **Interactive Mode**: Prompts for required fields like Name and Year if not provided.
- **Full CLI Support**: Supports arguments for non-interactive use and piping.

## Installation

### From Source

```bash
git clone https://github.com/teja/licgen.git
cd licgen
make
sudo make install
```

## Usage

### List Available Licenses
```bash
licgen list
```

### View License Details
See permissions, conditions, and limitations (like choosealicense.com):
```bash
licgen mit --help
```

### Generate a License

By default, `licgen` writes to a file in the current directory:

```bash
licgen mit                     # Interactive mode, writes to LICENSE.txt
licgen mit --name "John Doe"   # Non-interactive, writes to LICENSE.txt
```

### Output Formats

- **Text (Default)**: Writes to `LICENSE.txt`
- **Markdown**: Use `-m` or `--markdown`. Writes to `LICENSE.md`.
- **JSON**: Use `-j` or `--json`. Writes to `LICENSE.json`.

```bash
licgen mit --markdown    # Creates LICENSE.md
licgen mit --json        # Creates LICENSE.json
```

### Print to Terminal

If you want the output printed to the terminal instead of a file (e.g., for piping):

```bash
licgen mit --stdout      # Prints to terminal
licgen mit -s            # Short version
```

### Advanced Generation
```bash
licgen mit --name "Jane Doe" --year 2026 --output MY_LICENSE
# Using short flags:
licgen mit -n "Jane Doe" -y 2026 -o MY_LICENSE
```

## Adding New Licenses

Adding a new license is incredibly simple. Just create a `.template` file in the `templates/` directory:

```ini
ID: mylic
Name: My Custom License
SPDX: MY-LIC
Category: permissive
Description: A very cool license.

Permissions:
- Commercial use
- Private use

Conditions:
- License notice

---
Copyright (c) {{year}} {{name}}
... license text here ...
```

## Supported Licenses

The tool currently supports many popular licenses including:
- **Permissive**: MIT, Apache 2.0, BSD 2/3-Clause, ISC, Zlib, Boost
- **Copyleft**: GPL v3.0 (and more in progress)
- **Public Domain**: Unlicense, CC0, WTFPL

## License

This project is licensed under the MIT License.
