use std::fs::File;
use std::io::{self, BufRead};
use std::collections::HashSet;

#[derive(Clone, Debug)]
struct Big {
    digits: Vec<u64>,
}

impl Big {
    fn new() -> Self {
        Big { digits: Vec::new() }
    }

    fn from_u64(x: u64) -> Self {
        Big { digits: vec![x] }
    }

    fn add_assign(&mut self, other: &Big) {
        let mut carry: u128 = 0;
        let n = self.digits.len().max(other.digits.len());

        if self.digits.len() < n {
            self.digits.resize(n, 0);
        }

        for i in 0..n {
            let a = self.digits[i] as u128;
            let b = if i < other.digits.len() { other.digits[i] as u128 } else { 0 };
            let sum = a + b + carry;
            self.digits[i] = sum as u64;
            carry = sum >> 64;
        }

        if carry > 0 {
            self.digits.push(carry as u64);
        }

        while let Some(&0) = self.digits.last() {
            self.digits.pop();
        }
    }

    fn add_u64(&mut self, x: u64) {
        self.add_assign(&Big::from_u64(x));
    }
}

fn main() -> io::Result<()> {
    let path = std::env::args().nth(1).expect("Provide input file");
    let file = File::open(path)?;
    let lines: Vec<String> = io::BufReader::new(file).lines().collect::<Result<_, _>>()?;

    let rows = lines.len();
    let cols = lines[0].len();
    let pos_s = cols / 2;

    // --- Part 1 ---
    let mut part1 = 0;
    let mut curr: HashSet<usize> = HashSet::new();

    if rows > 1 { curr.insert(pos_s); }

    for r in 1..rows {
        let mut next: HashSet<usize> = HashSet::new();

        for &c in &curr {
            if c >= cols { continue; }

            if lines[r].as_bytes()[c] == b'^' {
                part1 += 1;
                if c > 0 { next.insert(c-1); }
                if c+1 < cols { next.insert(c+1); }
            } else {
                next.insert(c);
            }
        }

        curr = next;
    }

    // --- Part 2 ---
    let mut curr_b: Vec<Big> = vec![Big::new(); cols];
    let mut next_b: Vec<Big> = vec![Big::new(); cols];

    if rows > 1 { curr_b[pos_s].add_u64(1); }

    for r in 1..rows {
        for b in &mut next_b { b.digits.clear(); }

        for c in 0..cols {
            if curr_b[c].digits.is_empty() { continue; }

            if lines[r].as_bytes()[c] == b'^' {
                if c > 0 { next_b[c-1].add_assign(&curr_b[c]); }
                if c+1 < cols { next_b[c+1].add_assign(&curr_b[c]); }
            } else {
                next_b[c].add_assign(&curr_b[c]);
            }
        }

        std::mem::swap(&mut curr_b, &mut next_b);
    }

    // accumulate Part2
    let mut part2 = 0u64;
    let mut part2_big_needed = false;
    let mut part2_big = Big::new();

    for b in &curr_b {
        if !b.digits.is_empty() {

            if !part2_big_needed {

                if b.digits.len() > 1 || b.digits[0] > u64::MAX - part2 {
                    part2_big_needed = true;

                    if part2 != 0 { part2_big.add_u64(part2); part2 = 0; }
                }
            }

            if part2_big_needed {
                part2_big.add_assign(b);
            } else {
                part2 += b.digits[0];
            }
        }
    }

    println!("part1: {}", part1);
    if part2_big_needed {

        if !part2_big.digits.is_empty() {
            println!("part2: {}", part2_big.digits[0]); // simple print
        } else {
            println!("part2: 0");
        }
    } else {
        println!("part2: {}", part2);
    }

    Ok(())
}
