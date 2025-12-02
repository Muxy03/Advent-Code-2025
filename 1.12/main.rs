use std::env;
use std::fs::File;
use std::io::{BufRead, BufReader};

fn main() {
	let args: Vec<String> = env::args().collect();
	
    if args.len() < 2 {
        eprintln!("Usage: {} <input_file>", args[0]);
        std::process::exit(1);
    }

    let file = File::open(&args[1]).unwrap();
    let reader = BufReader::new(file);
    let mut state: i32 = 50;
    let mut part1 = 0;
    let mut part2 = 0;
    
    for line in reader.lines() {
        let line = line.unwrap();
        
        if let Some(dir) = line.chars().next() {
            assert!(dir == 'L' || dir == 'R');
            if let Ok(distance) = line[1..].parse::<i32>() {

                let mut tmp = if dir == 'R' { 
                    (100 - state) % 100 
                } else { 
                    state % 100 
                };
                
                if tmp == 0 { 
                    tmp = 100; 
                }
                
                state = if dir == 'R' { 
                    (state + distance) % 100 
                } else { 
                    (state - distance) % 100 
                };

                if state < 0 { 
                    state += 100; 
                }
                
                if distance >= tmp { 
                    part2 += 1 + (distance - tmp) / 100; 
                }

                if state == 0 { 
                    part1 += 1; 
                }
            }
        }
    }
    
    println!("part1: {part1}");
    println!("part2: {part2}");
}
