use std::env;
use std::fs::File;
use std::io::{self, BufRead, BufReader};

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() != 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1])?;
    let reader = BufReader::new(file);

    // Read file into matrix
    let mut matrix: Vec<Vec<char>> = Vec::new();
    for line in reader.lines() {
        let line = line?;
        matrix.push(line.chars().collect());
    }

    let size = matrix.len() as i32;

    let dr = [-1, -1, -1,  0, 0, 1, 1, 1];
    let dc = [-1,  0,  1, -1, 1,-1, 0, 1];

    let mut part1: usize = 0;
    let mut part2: usize = 0;

    // =======================
    // PART 1
    // =======================
    for i in 0..size {
        for j in 0..size {
            if matrix[i as usize][j as usize] != '@' {
                continue;
            }

            let mut neighbors = 0;

            for k in 0..8 {
                let nr = i + dr[k];
                let nc = j + dc[k];

                if nr >= 0 && nr < size && nc >= 0 && nc < size {
                    if matrix[nr as usize][nc as usize] == '@' {
                        neighbors += 1;
                    }
                }
            }

            if neighbors < 4 {
                part1 += 1;
            }
        }
    }

    println!("Part1: {}", part1);

    // =======================
    // PART 2
    // =======================
    loop {
        let mut round_removed: usize = 0;

        for i in 0..size {
            for j in 0..size {
                if matrix[i as usize][j as usize] != '@' {
                    continue;
                }

                let mut neighbors = 0;

                for k in 0..8 {
                    let nr = i + dr[k];
                    let nc = j + dc[k];

                    if  nr >= 0
                        && nr < size
                        && nc >= 0
                        && nc < size
                        && matrix[nr as usize][nc as usize] == '@'
                    {
                        neighbors += 1;
                    }
                }

                if neighbors < 4 {
                    matrix[i as usize][j as usize] = '.';
                    round_removed += 1;
                }
            }
        }

        if round_removed == 0 {
            break;
        }

        part2 += round_removed;
    }

    println!("Part2: {}", part2);

    Ok(())
}
