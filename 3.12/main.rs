use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};

fn part1(line: &str) -> i64 {
    let len = line.len(); // FIXED
    let bytes = line.as_bytes();
    let mut max_joltage: i64 = 0;

    for i in 0..len {
        for j in (i + 1)..len {
            let joltage =
                ((bytes[i] - b'0') as i64) * 10 + (bytes[j] - b'0') as i64;
            if joltage > max_joltage {
                max_joltage = joltage;
            }
        }
    }

    max_joltage
}

fn part2(line: &str) -> i64 {
    let len = line.len(); // FIXED
    if len < 12 {
        return 0;
    }

    let mut digits_to_remove = len - 12;
    let mut stack: Vec<u8> = Vec::with_capacity(len);

    for &current in line.as_bytes() {
        while !stack.is_empty()
            && digits_to_remove > 0
            && *stack.last().unwrap() < current
        {
            stack.pop();
            digits_to_remove -= 1;
        }
        stack.push(current);
    }

    let final_len = stack.len() - digits_to_remove;
    stack.truncate(final_len);

    let mut joltage: i64 = 0;
    for i in 0..12 {
        joltage = joltage * 10 + (stack[i] - b'0') as i64;
    }

    joltage
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let reader = BufReader::new(file);

    let mut part1_sum: i64 = 0;
    let mut part2_sum: i64 = 0;

    for line in reader.lines() {
        let line = line?;
        if !line.is_empty() {
            part1_sum += part1(&line);
            part2_sum += part2(&line);
        }
    }

    println!("Part 1: {}", part1_sum);
    println!("Part 2: {}", part2_sum);

    Ok(())
}
