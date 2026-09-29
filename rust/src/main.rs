use std::{env, fs};
use std::io::{self, BufWriter, Write};
use std::process::ExitCode;

fn main() -> ExitCode {
  let args: Vec<String> = env::args().collect();

  if args.len() != 2 {
    eprintln!("Usage: {} <file>", args[0]);
    return ExitCode::FAILURE;
  }

  match run(&args[1]) {
    Ok(()) => ExitCode::SUCCESS,
    Err(e) => {
      eprintln!("{}: {e}", args[1]);
      ExitCode::FAILURE
    }
  } 
}

fn run(path: &str) -> io::Result<()> {
  let buf = fs::read(path)?;
  hexdump(&buf)
}

fn hexdump(buf: &[u8]) -> io::Result<()> {
  let stdout = io::stdout();
  let mut out = BufWriter::new(stdout.lock());

  for (i, chunk) in buf.chunks(16).enumerate() {
    write!(out, "{:08x}  ", i * 16)?;
    
    for (j, byte) in chunk.iter().enumerate() {
      write!(out,"{:02x} ", byte)?;
      if j == 7 {
        write!(out," ")?;
      }
    }

    if chunk.len() < 16 {
      for _ in 0..(16 - chunk.len()) {
        write!(out,"   ")?;
      }
      if chunk.len() < 8 {
        write!(out," ")?;
      }
    }

    write!(out," |")?;

    for &byte in chunk {
      if byte.is_ascii_graphic() || byte == b' ' {
        write!(out,"{}", byte as char)?;
      } else {
        write!(out,".")?;
      }
    }

    writeln!(out, "|")?;
  }
  
  if !buf.is_empty() {
    writeln!(out, "{:08x}", buf.len())?;
  }
  
  out.flush()?;
  Ok(())
}
