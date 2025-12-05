use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};

#[derive(Debug, Clone)]
struct Range {
    left: i64,
    right: i64,
}

fn merge_ranges(ranges: &mut Vec<Range>) -> Vec<Range> {
    if ranges.is_empty() {
        return Vec::new();
    }

    ranges.sort_by_key(|r| r.left);
    let mut merged = vec![ranges[0].clone()];

    for r in ranges.iter().skip(1) {
        let last = merged.last_mut().unwrap();
        if r.left <= last.right + 1 {
            last.right = last.right.max(r.right);
        } else {
            merged.push(r.clone());
        }
    }

    merged
}

fn is_fresh(ranges: &[Range], id: i64) -> bool {
    let mut left = 0;
    let mut right = ranges.len() as isize - 1;

    while left <= right {
        let mid = left + (right - left) / 2;
        let mid_range = &ranges[mid as usize];
        if mid_range.left <= id && id <= mid_range.right {
            return true;
        } else if id < mid_range.left {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    false
}

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let reader = BufReader::new(file);

    let mut ranges: Vec<Range> = Vec::new();
    let mut elements: Vec<i64> = Vec::new();
    let mut reading_elements = false;

    for line_res in reader.lines() {
        let line = line_res?;
        let line = line.trim();

        if line.is_empty() {
            reading_elements = true;
            continue;
        }

        if reading_elements {
            elements.push(line.parse().expect("Invalid number"));
        } else {
            let parts: Vec<&str> = line.split('-').collect();
            if parts.len() != 2 {
                eprintln!("Malformed range '{}'", line);
                std::process::exit(1);
            }
            let left = parts[0].parse().expect("Invalid left number");
            let right = parts[1].parse().expect("Invalid right number");
            ranges.push(Range { left, right });
        }
    }

    let merged = merge_ranges(&mut ranges);
    elements.sort_unstable();

    let part1 = elements.iter().filter(|&&e| is_fresh(&merged, e)).count();
    let part2: i64 = merged.iter().map(|r| r.right - r.left + 1).sum();

    println!("part1: {}", part1);
    println!("part2: {}", part2);

    Ok(())
}
