# PCBID Tools

![Akornsys RDI](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/akornsys-logo.png)
![OSHW PDC](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/oshw-logo.png)

## PCBID management tools

PCBID Tools is a set of different tools oriented to the management of PCBID unique identifiers. PCBID Tools follows the PCBID V2 Specifications that can be found in [PCBID Specifications](https://github.com/akornsys-rdi/pcbid-specifications).

PCBID Tools contains the following tools:

- `pcbid-generator`: PCBID numbering generator. Edit and add entries to `projects.txt`.
- `algorithm-test`: Documentation of dispersion tests.

## :open_file_folder: Repository content

PCBID Tools can be obtained on GitHub: [PCBID Tools](https://github.com/akornsys-rdi/pcbid-tools "PCBID Tools").

This project may contain some of the following sections:

### Hardware

- `/brd/` EDA designs, schematics, PCBs, libraries, datasheets and manufacturing files such as gerber, BoM and PDF.

### Software

- `/src/` Firmware or software files, source and compiled.

### Documentation

- `/doc/` Written or graphic documentation.
- Any file containing project information at the root of the project, including the README.
- Wiki or gh-pages if available.

## :rocket: Getting started

PCBID Tools is formed by different tools, being the main one `pcbid-generator`. The **Install** and **Usage** sections only contemplate the instructions for this tool. Additional sections deal with the rest of tools provided by this repository. The rest of the tools which complement `pcbid-generator` do not have an installation method and have to be copied manually if desired.

The `pcbid-generator` allows to do all the usual functions in the management of PCBID identifiers: Create a new author and project, create a new project for an existing author, modify module or release for an existing project, or add an entry to the `projects.txt` file.

### Install

In order to compile `pcbid-generator`, OpenSSL's `libcrypto` library, provided by the `libssl-dev` package on Debian systems and derivatives, is required:

```bash
sudo apt-get install libssl-dev
```

Installation is done by downloading a copy of the repository and compiling the source code:

```bash
git clone https://github.com/akornsys-rdi/pcbid-tools.git
cd pcbid-tools/src/
make
sudo make install
```

By default, `pcbid-generator` is installed in `/usr/local/bin`, if you want to change the installation path you can set `PREFIX` to the desired path:

```bash
make install PREFIX=$HOME/bin
```

### Usage

`pcbid-generator` is a command line tool designed to manage PCBID identifiers. It has the functionalities described below:

```bash
Usage: pcbid-generator {-h | -v}
  or   pcbid-generator -i [OPTIONS]
  or   pcbid-generator --author-name <EXP> --country <EXP> --project-name <EXP> --module <EXP> --release <EXP> --date <EXP> [OPTIONS]
Generate unique numbers for PCBs.
Try `pcbid-generator --help` for more information.

Options
      --author-name <EXP>       Set author name to <EXP>
      --collision-file <FILE>   Set collision file to <FILE>
      --country <EXP>           Set country to <EXP> (ISO3166-1-A2)
      --date <EXP>              Set date to <EXP> (YYYY-MM-DD)
  -d, --dry-run                 Prevent overwriting files
  -h, --help                    Display this help and exit
  -i, --interactive             Interactive mode
      --module <EXP>            Set module to <EXP> (decimal)
      --project-name <EXP>      Set project name to <EXP>
  -q, --quiet                   Suppress verbose messages
      --release <EXP>           Set release to <EXP> (decimal)
  -r, --remove-tilde            Remove heading tilde
  -v, --version                 Output version information and exit
```

As you can read in the PCBID V2 Specifications, it is important that the author name and project name fields are correctly fulfilled:

> :warning: By style convention intended for the readability and accuracy of the data provided, the author name and project name fields must present full names, without unnecessary abbreviations and with the correct capitalization and punctuation.

#### Interactive mode

The interactive mode allows to provide the necessary data to generate a new PCBID by answering the questions elaborated by the program. To invoke this mode, the `-i` argument must be used:

```bash
pcbid-generator -i
```

Other arguments can be used in the interactive mode. The arguments of the automatic mode provide default values to be used in the interactive mode.

The interactive mode allows to realize all the functionalities of PCBID:

- Create new author: When asked if you already have an author identifier, answer **no** to generate a new one. This will check for possible collisions to ensure that a unique identifier is generated.
- Create new project from an existing author: When asked if you already have an author identifier, answer **yes** to skip the collision check. You must be assigned the same author identifier you already have. When asked if you already have a project identifier, answer **no** to generate a new one. This will check for possible collisions to ensure that a unique identifier is generated.
- Update the module or release of an existing project: When asked if you already have an author and project identifier, answer **yes** to both. You must be assigned the same author and project identifier you already have.
- Add entry to `projects.txt`: If the `--collision-file` argument is provided, collisions with this file will be automatically checked and the newly generated entry will be added. More information is available in the relevant sections of this document.

If you already have an author and/or project identifier and the generated identifier does not match the one you already have, repeat the process and make sure you enter the author or project name correctly. The author or project name must be identical to the previously entered one, any minimal change will produce a different identifier.

If you still cannot obtain the identifier you already have, modify the PCBID obtained with the identifier you already have and use this latter one. Edit the `projects.txt` file to make the modified one appear. Open an issue in the repository of the PCBID generation tool to solve the problem.

#### Automatic mode

The automatic mode allows to provide the necessary data to generate a new PCBID by means of command line arguments. To invoke this mode it is necessary to use the following arguments:

```bash
pcbid-generator --author-name <EXP> --country <EXP> --project-name <EXP> --module <EXP> --release <EXP> --date <EXP>
```

Other arguments can be used in the automatic mode. More information is available in the relevant sections of this document.

It is necessary to replace `<EXP>` with the appropriate expression for each argument:

- `--author-name`: Full author name for the generation of the author identifier. It is convenient to delimit the field of the expression by single quotes `''`.
- `--country`: [ISO3166-1-A2](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2) country code to which the author belongs.
- `--project-name`: Full project name for the generation of the project identifier. It is convenient to delimit the field of the expression by single quotes `''`.
- `--module`: Decimal number of the module (PCB number) of the project.
- `--release`: Decimal number of the project release.
- `--date`: Date of completion of the PCB, in ISO 8601 format (YYYY-MM-DD).

More information about these fields in the PCBID Specifications.

Example:

```bash
pcbid-generator --author-name 'Foobar Inc' --country ZZ --project-name 'Foobar Project' --module 1 --release 1 --date 2020-04-11
```

If you already have an author and/or project identifier and the generated identifier does not match the one you already have, repeat the process and make sure you enter the author or project name correctly. The author or project name must be identical to the previously entered one, any minimal change will produce a different identifier.

If you still cannot obtain the identifier you already have, modify the PCBID obtained with the identifier you already have and use this latter one. Edit the `projects.txt` file to make the modified one appear. Open an issue in the repository of the PCBID generation tool to solve the problem.

#### `--collision-file` and `--dry-run`

The `--collision-file` argument allows to provide a path for the [`projects.txt`](https://github.com/akornsys-rdi/pcbid-specifications/blob/master/projects.txt) file, which indexes all the identifiers generated so far, and which is stored in the PCBID Specification repository. This allows to avoid collisions with existing identifiers, and to automatically add the newly generated identifier to this file.

The use of the `--collision-file` argument changes the operation of the automatic mode, so it should only be used when a new identifier is generated and not when you intend to change the module or release number of an existing identifier.

When the `--collision-file` argument is not provided, the program generates a message to manually insert the entry in the `projects.txt` file. This should only be taken into account in procedures for generating new identifiers, but not when you intend to change the module or release number of an existing identifier.

The `--collision-file` argument processes the file provided and only supports files that contain valid entries or are empty. Also, in the process of updating the file, invalid lines are removed and existing entries are sorted to follow the style convention of the file.

The file can be used in read-only mode, so its entries will be taken into account to look for collisions but its contents will not be inserted or modified, using the `--dry-run` argument.

When modifying the `projects.txt` file, using the PCBID generation tool or manually, be sure to perform a pull-request in the [PCBID Specifications](https://github.com/akornsys-rdi/pcbid-specifications) repository to share the changes.

#### `--quiet` and `--remove-tilde`

The `--quiet` argument removes all information messages to the user, which is very useful in automatic mode for using the program in scripts or pipelining. With this argument, the program only generates the PCBID, without any other messages or lines. Warning or error messages are displayed using the `stderr` output, so they do not interfere with the `stdout` output.

By default, the generated PCBID numbering goes with the symbol tilde '~' in the number header, according to the PCBID Specifications. This character can be removed using the `--remove-tilde` argument.  In combination with the `--quiet` argument it can be useful in automated processes.

#### Barcode generation

It is possible to generate barcodes, according to PCBID Specifications, by combining the PCBID generation program with the `libdmtx` library. On Debian systems and derivatives it can be installed using:

```bash
sudo apt-get install libdmtx0a libdmtx-utils
```

Using the `dmtxwrite` utility, it is possible to pipe the output of the PCBID generation tool to generate datamatrix type barcodes. Example:

```bash
./pcbid-generator --author-name 'Foobar Inc' --country ZZ --project-name 'Foobar Project' --module 1 --release 1 --date 2020-04-11 -r -q | dmtxwrite -m 10 -d 10 -s s -o pcbid-dm.png
```

### Algorithm-test

`algorithm-test` was originally created to test the dispersion of different hash transformation algorithms. It currently implements the step-by-step process of creating the identifier from a hash. It can be used to manually generate an identifier field from a hash, or to check the results of each step in creating your own tools.

It does not generate the hash from a text string, so it has to be generated beforehand. On GNU/Linux systems it can be generated using:

```bash
echo -n "Foobar Inc" | sha256sum
```

### PCBID Card

`pcbid-card` is a PCB for example of use of PCBID. It contains sample elements of PCBID and common elements in PCB cards:

- Ruler: Two-sided 85 mm (3.346 inch) ruler, with a scale in millimeters and inches, with a resolution of half a millimeter and 25 thousand.
- URL: URL links to relevant PCBID web addresses.
- 2D Barcode: PCBID table in datamatrix and text format with samples at different sizes in positive and negative.
- PCB Art Palette: Palette of layers of PCB materials.
- Lanyard hole: Side hole in the card to insert a lanyard.
- Trace width sample: Layouts of traces with different trace and clearance sizes.
- Trace and via table: Table that crosses the current capacity, maximum voltage and resistance with the size of traces and via for usual types of layouts.
- Silkscreen size: Sample of different silkscreen sizes, with their common uses.
- K85 Charset: The character set of K85 at different screen sizes to check its readability.
- Maze layout: Provides a background with horizontal and vertical traces to simulate the readability of silkscreen on a conventional PCB. The trace that draws this layout has a width of 250µm and a length of 7050mm.

## :book: Version history

PCBID Tools uses [Semantic Versioning](http://semver.org/ "Semantic Versioning"). For the versions available, see the [tags on this repository](https://github.com/akornsys-rdi/pcbid-tools/releases).

PCBID Tools version [1.2.0](https://github.com/akornsys-rdi/pcbid-tools/releases/tag/v1.2.0) is the latest stable version.

The version of PCBID Tools refers to the version of `pcbid-generator`, for the other elements of the set of tools no changes are contemplated and they are published as a complement to `pcbid-generator`.

All changes made prior to version 1.2.0 were made as part of development and have not been published.

### V1.3.0

- Created `pcbid-card` tool.

### V1.2.0

- Added file management and verification features.
- Added `--country`, `--collision-file` and `--dry-run` command line options.
- Removed `-author-id` and `-project-id` command line options.
- Modified the operating cycle of the interactive mode to accommodate the new features.

### V1.1.1

- Added empty string warning messages for author and project names in interactive and automatic mode.

### V1.1.0

- Added `--author-id`, `--author-name`, `--project-id`, `--project-name`, `--release`, `--module`, `--date`, `--quiet` and `--remove-tilde` command line options.
- Added automatic mode.

### V1.0.0

- Created `pcbid-generator` program with interactive mode.
- Created `algorithm-test` tool.

## :heart: Community and contributions

[![Donate using Liberapay](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/liberapay-donate.png)](https://liberapay.com/RileyStarlight/donate)

Contributions are always welcome!

This is a **community-driven open source project**. Any contribution is highly appreciated, whether you are helping [fixing bugs or proposing new features](https://github.com/akornsys-rdi/pcbid-tools/issues "Issues"), [pulling requests](https://github.com/akornsys-rdi/pcbid-tools/pulls "Pull Requests"), improving the documentation or spreading the word.

Please :star: or watch this repository if this project helped you! You can also support this project on [Liberapay](https://liberapay.com/RileyStarlight/donate).

## :scroll: Copyright & License

Copyright © 2020 RileyStarlight

### Hardware

[![Cern OHL-S 2](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/cernohl-logo.png)](https://cern.ch/cern-ohl)

The hardware of this project is licensed under CERN-OHL-S 2, see the [`LICENSE.OHLS2.txt`](LICENSE.OHLS2.txt) file for details.

### Software

[![GNU/GPL 3](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/gnugpl-logo.png)](https://www.gnu.org/licenses/gpl.html "GNU General Public License")

The software of this project is licensed under GNU/GPL 3, see the [`LICENSE.GPL3.txt`](LICENSE.GPL3.txt) file for details.

### Documentation

[![CC BY SA 4.0](https://github.com/akornsys-rdi/pcbid-tools/raw/master/doc/img/ccbysa-logo.png)](https://creativecommons.org/licenses/by-sa/4.0/ "Creative Commons BY-SA 4.0")

The documentation of this project is licensed under CC BY SA 4, see the [`LICENSE.BYSA4.txt`](LICENSE.BYSA4.txt) file for details.

