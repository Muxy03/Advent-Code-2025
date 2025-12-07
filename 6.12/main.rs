use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::str;

// Parse a string containing digits into i64
fn parse_ll(s: &str) -> i64 {
    let mut v: i64 = 0;
    for b in s.bytes() {
        if b.is_ascii_digit() {
            v = v * 10 + (b - b'0') as i64;
        }
    }
    v
}

fn main() -> io::Result<()> {

    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let reader = BufReader::new(file);

    let mut lines: Vec<String> = Vec::new();
    let mut cols = 0;

    for line_res in reader.lines() {

        let line = line_res?;

        if line.len() > cols {
            cols = line.len();
        }

        lines.push(line);
    }

    let rows = lines.len();

    for line in &mut lines {
        while line.len() < cols {
            line.push(' ');
        }
    }

    let op_row = rows - 1;
    let mut col_has = vec![false; cols];

    for c in 0..cols {
        for r in 0..op_row {

            if lines[r].as_bytes()[c] != b' ' {
                col_has[c] = true;
                break;
            }
        }
    }

    let mut part1: i64 = 0;
    let mut part2: i64 = 0;

    let mut c = 0;
    while c < cols {

        while c < cols && !col_has[c] {
            c += 1;
        }

        if c >= cols {
            break;
        }

        let l = c;

        while c < cols && col_has[c] {
            c += 1;
        }

        let r = c - 1;

        let mut op = b'+';

        for cc in l..=r {

            let ch = lines[op_row].as_bytes()[cc];

            if ch != b' ' {
                op = ch;
            }
        }

        // -------- Part 1: Top-to-bottom column numbers --------
        let mut acc1 = if op == b'*' { 1 } else { 0 };
        let mut used1 = false;

        for row in 0..op_row {

            let len = r - l + 1;
            let sub = &lines[row][l..l + len];
            let trim = sub.trim();

            if !trim.is_empty() {
                let v = parse_ll(trim);
                used1 = true;

                if op == b'+' {
                    acc1 += v;
                } else {
                    acc1 *= v;
                }
            }
        }

        if used1 {
            part1 += acc1;
        }

        // -------- Part 2: Right-to-left column numbers --------
        let mut acc2 = if op == b'*' { 1 } else { 0 };
        let mut used2 = false;

        for cc in (l..=r).rev() {
            let mut buf: Vec<u8> = Vec::new();

            for row in 0..op_row {
                let ch = lines[row].as_bytes()[cc];

                if ch != b' ' {
                    buf.push(ch);
                }
            }
            if !buf.is_empty() {
                let s = str::from_utf8(&buf).unwrap();
                let v = parse_ll(s);
                used2 = true;

                if op == b'+' {
                    acc2 += v;
                } else {
                    acc2 *= v;
                }
            }
        }
        
        if used2 {
            part2 += acc2;
        }
    }

    println!("Part 1: {}", part1);
    println!("Part 2: {}", part2);

    Ok(())
}
