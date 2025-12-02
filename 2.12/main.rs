use std::fs::File;
use std::io::{BufRead, BufReader};
use std::error::Error;

fn is_double_repeat(s: &str) -> bool {
    let n = s.len();
    if n % 2 != 0 { return false; }
    let half = n / 2;
    &s[..half] == &s[half..]
}

fn is_multi_repeat(s: &str) -> bool {
    let n = s.len();
    for len in 1..=n / 2 {
        if n % len != 0 {
            continue;
        }
        let repeat = n / len;
        if repeat < 2 {
            continue;
        }
        let pat = &s[..len];

        if (1..repeat).all(|r| &s[r * len..(r + 1) * len] == pat) {
            return true;
        }
    }
    false
}

fn main() -> Result<(), Box<dyn Error>> {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        return Ok(());
    }

    let file = File::open(&args[1])?;
    let mut reader = BufReader::new(file);

    let mut line = String::new();
    if reader.read_line(&mut line)? == 0 {
        eprintln!("Error: input file is empty");
        return Ok(());
    }

    let line = line.trim();

    let mut part1: i64 = 0;
    let mut part2: i64 = 0;

    for token in line.split(',') {
        let mut parts = token.split('-');
        let a_str = parts.next();
        let b_str = parts.next();

        let (a, b) = match (a_str, b_str) {
            (Some(as_), Some(bs_)) => {
                let a_val = as_.parse::<i64>();
                let b_val = bs_.parse::<i64>();
                if a_val.is_err() || b_val.is_err() {
                    eprintln!("Warning: could not parse range '{}'", token);
                    continue;
                }
                (a_val.unwrap(), b_val.unwrap())
            }
            _ => {
                eprintln!("Warning: malformed range '{}'", token);
                continue;
            }
        };

        for x in a..=b {
            let s = x.to_string();

            if is_double_repeat(&s) {
                part1 += x;
            }
            if is_multi_repeat(&s) {
                part2 += x;
            }
        }
    }

    println!("Part1: {}", part1);
    println!("Part2: {}", part2);

    Ok(())
}
