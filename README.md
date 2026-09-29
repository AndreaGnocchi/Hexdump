# Hexdump

A small command-line hex dump utility that reads a file and prints its contents as `hexdump -C`-style offset/hex/ASCII rows. It's implemented twice, once in **C** (on top of [AGLib](https://github.com/AndreaGnocchi/AGLib/tree/main)) and once in **Rust** (standard library only), so the two can be compared side by side.

## Project layout

```
Hexdump/
├── c/          # C implementation (uses AGLib as a git submodule)
│   ├── main.c
│   ├── Makefile
│   └── AGLib/  # submodule
└── rust/       # Rust implementation (no external dependencies)
    ├── Cargo.toml
    └── src/main.rs
```

## Output format

Both versions match `hexdump -C` byte for byte. Each row is:

- an 8-digit hex offset, followed by two spaces,
- 16 hex byte values, with an extra space between the two groups of 8,
- a right-hand column showing the same bytes as ASCII, with `.` for anything that isn't printable (printable means `0x20`-`0x7e`).

Example:

```
$ ./hexdump README.md
00000000  23 20 48 65 78 64 75 6d  70 0a 0a 41 20 73 6d 61  |# Hexdump..A sma|
...
```

## C version

### How it works

- **Arena-based memory.** A single `sArena` is initialized up front (sized to hold the read buffer plus file-reading overhead), and all buffers used by the program are carved out of it, so there is no manual `malloc`/`free` bookkeeping.
- **File loading.** `open_read_file` validates the path (`path_exists`/`is_file`), reads the entire file via AGLib's `read_all_file` into the arena, and copies it into a fixed-size working buffer.
- **Formatting.** `hexdump` walks the buffer 16 bytes at a time, printing the row offset, the hex values (with an extra space every 8 bytes and padding on a short final row), and the ASCII rendering of that row.

### Building

The `Makefile` lives in `c/`. AGLib is included as a git submodule at `c/AGLib`, so fetch it first if you cloned without `--recurse-submodules`:

```sh
git submodule update --init
cd c
make
./hexdump <file>
```

`make` builds AGLib automatically if `libag.a` doesn't exist yet. If AGLib lives somewhere else, override the location:

```sh
make AGLIB_DIR=/path/to/AGLib
```

Only Linux and macOS are supported (the program uses POSIX `open()`/`fcntl`). Use `make clean` to remove the binary.

### Limitations

Files larger than **1 MiB** are rejected, since the whole file is read into a fixed-size buffer (`BUF_SIZE`, and the arena is sized to match). Increase `BUF_SIZE` in `main.c` if you need more.

## Rust version

### How it works

The Rust version reads the whole file with `std::fs::read`, then iterates over it with `chunks(16)`, writing each row through a locked, buffered stdout. Errors (e.g. a missing file) are reported as `<file>: <error>` with a non-zero exit code. There is no fixed file-size limit beyond available memory.

### Building

Requires Rust 1.85 or newer (the crate uses the 2024 edition):

```sh
cd rust
cargo build --release
./target/release/hexdump <file>
```

or run it directly:

```sh
cargo run --release -- <file>
```

## Usage

```
hexdump <file>
```

Exactly one argument is expected; otherwise a usage message is printed and the program exits with a failure status.

## Dependencies

- **C:** [AGLib](https://github.com/AndreaGnocchi/AGLib/tree/main), for the arena allocator (`sArena`, `arena_init`, `arena_alloc`) and file utilities (`path_exists`, `is_file`, `read_all_file`).
- **Rust:** none (standard library only).