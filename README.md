# C++ Command-Line Shell Interpreter

A simplified command-line interpreter (CLI shell) written in C++, built as the project assignment for the **Object-Oriented Programming** course at the School of Electrical Engineering, University of Belgrade.

It reads commands from an input stream, parses them according to a custom lexical grammar, and executes them - supporting quoted arguments, I/O redirection, pipes, and batch scripts, much like a stripped-down version of `bash` or `cmd`.

## Features

- **Interactive REPL** - prints a prompt, reads a line, parses it, executes it, repeats (`Shell::run`).
- **11 built-in commands**: `echo`, `prompt`, `time`, `date`, `touch`, `truncate`, `rm`, `wc`, `tr`, `head`, `batch`.
- **Custom lexer/parser** - tokenizes each line (max 512 characters), handles quoted strings, whitespace separation, and flags illegal characters with a caret (`^`) pointing at the exact offending position.
- **I/O redirection** - `<` for input, `>` / `>>` for output (overwrite / append).
- **Pipes** - chain multiple commands together (`cmd1 | cmd2 | cmd3`), passing output from one straight into the next.
- **Batch execution** - the `batch` command runs every line of a text file as if it were typed interactively, continuing past individual line errors.
- **Structured error handling** - lexical, syntactic, semantic, and OS-level errors are all caught and reported without crashing the shell, via a small custom exception hierarchy.

## Example

```
$ wc -w "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
8
$ echo "I love programming in C!" | tr -"C" "C++" | wc -c
26
$ time | tr -":" "." | wc -c > time.txt
```

## Architecture

The design centers around a small **Command** hierarchy plus a **Shell** that owns and orchestrates it:

```
Command (abstract)
 ├─ EchoCommand, PromptCommand, TimeCommand, DateCommand      (SystemCommands)
 ├─ TouchCommand, TruncateCommand, RmCommand                  (FileCommands)
 ├─ TextCommand (abstract)
 │    ├─ WcCommand, TrCommand, HeadCommand                    (TextCommands)
 └─ BatchCommand
```

| Class | Responsibility |
|---|---|
| `Command` | Abstract base for every command; owns parsed parameters and shared helpers (`fileExists`, `isOption`). |
| `Parser` | Static lexer/parser — turns a raw line into tokens, then into a pipeline of `CommandRun` structs. |
| `Shell` | Registers all commands, runs the read-parse-execute loop, wires up input/output streams (console, file, or pipe) for each stage of a pipeline. |
| `Errors` | `ShellError` base exception, with `LexicalError` and `RuntimeError` specializations. |
| `BatchCommand` | Reads a file and feeds each line back through the parser/executor, holding a back-pointer to `Shell`. |
| `SystemCommands`, `FileCommands`, `TextCommands` | Concrete command implementations grouped by purpose. |

Key C++/OOP concepts applied: abstract base classes and polymorphism, inheritance hierarchies, exception handling with a custom hierarchy, RAII-style file stream handling, and stream-based I/O abstraction that treats console, file, and pipe input/output uniformly.

## Project structure

```
cpp-shell-interpreter/
├── include/          # Header files (.h)
├── src/              # Implementation files (.cpp)
└── README.md
```

## Building

The project is plain, portable C++ (no external dependencies) and was developed in Visual Studio. To build it:

**Visual Studio**: create a new empty C++ project, add everything from `include/` and `src/`, and build.

**g++ / command line**:
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o shell
./shell
```

> Note: `SystemCommands.cpp` uses `localtime_s`, which is a Windows/MSVC-specific function. On Linux/macOS, swap it for `localtime_r` if you want to build outside of Visual Studio.


## Background

Built for the 2025/2026 Object-Oriented Programming course. 
