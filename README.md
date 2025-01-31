# CD_CDKTool
**Crashday Package Tool (CDKTool)** - a tool for CPK (Crashday Package) files, which were originally used to install mods for "classic" Crashday from 2006 via "CarManager" tool from SDK.
> [!WARNING]
> The tool is not made for the Redline Edition! For Redline Edition use ZIP-files with renamed ".CPK" file extension.

To work with the program, you must use the *Command Prompt* or specify the arguments in advance in the *.BAT file*.

```
Usage: CD_CPKTool.exe <COMMAND>

Commands:
  list          List all files in the archive [aliases: l, ls]
  unpack        Unpack / extract files from the archive [aliases: extract, ex, e, x, u, up]
  pack          Pack / archive files from the archive [aliases: archive, ar, a, p]
  help          Print help information [aliases: h]
  version       Print version information [aliases v]
```

For commands, below is how to use them (arguments in square brackets are optional):

```
Usage: CD_CPKTool.exe <COMMAND>

Commands:
  list <CPK_NAME>
  unpack <CPK_NAME> [OUTPUT_DIR] [TRKDATA_DIR]    Default value for output directory is input CPK file name. For car folder name is null.
  pack <INPUT_DIR> [CPK_NAME]                     Default value for output CPK file name is input folder with ".CPK" file extension.
```
