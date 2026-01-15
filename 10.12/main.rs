use std::fs::File;
use std::io::{self, BufRead};

// ============== Part 1: XOR Toggle ==============

fn extract_lights(line: &str) -> Option<u64> {
    let start = line.find('[')?;
    let end = line.find(']')?;
    let lights_str = &line[start + 1..end];
    
    Some(lights_str.chars().enumerate()
        .filter(|(_, c)| *c == '#')
        .fold(0u64, |acc, (i, _)| acc | (1 << i)))
}

fn extract_buttons(line: &str) -> Vec<u64> {
    let mut buttons = Vec::new();
    let mut start = 0;
    
    while let Some(pos) = line[start..].find('(') {
        let abs_start = start + pos;
        if let Some(end) = line[abs_start..].find(')') {
            let abs_end = abs_start + end;
            let button_str = &line[abs_start + 1..abs_end];
            
            let button = button_str.split(',')
                .filter_map(|n| n.trim().parse::<usize>().ok())
                .fold(0u64, |acc, i| acc | (1 << i));
            buttons.push(button);
            
            start = abs_end + 1;
        } else {
            break;
        }
    }
    buttons
}

fn solve_part1(line: &str) -> Option<usize> {
    let target = extract_lights(line)?;
    let buttons = extract_buttons(line);
    
    if buttons.is_empty() { return None; }
    
    let mut min_presses = usize::MAX;
    for combo in 0..(1u64 << buttons.len()) {
        let state: u64 = buttons.iter().enumerate()
            .filter(|(i, _)| (combo >> i) & 1 == 1)
            .fold(0, |acc, (_, &btn)| acc ^ btn);
        
        if state == target {
            min_presses = min_presses.min(combo.count_ones() as usize);
        }
    }
    Some(min_presses).filter(|&x| x != usize::MAX)
}

// ============== Part 2: Additive Counters ==============

#[derive(Clone, Copy)]
struct Rat(i64, i64);

impl Rat {
    fn new(n: i64, d: i64) -> Self {
        if d == 0 { return Rat(n, 1); }
        let g = gcd(n.abs(), d.abs());
        if d < 0 { Rat(-n / g, -d / g) } else { Rat(n / g, d / g) }
    }
    fn zero() -> Self { Rat(0, 1) }
    fn int(v: i64) -> Self { Rat(v, 1) }
    fn is_zero(&self) -> bool { self.0 == 0 }
    fn to_int(&self) -> Option<i64> {
        if self.1 == 1 { Some(self.0) }
        else if self.0 % self.1 == 0 { Some(self.0 / self.1) }
        else { None }
    }
    fn add(self, o: Rat) -> Rat { Rat::new(self.0 * o.1 + o.0 * self.1, self.1 * o.1) }
    fn sub(self, o: Rat) -> Rat { Rat::new(self.0 * o.1 - o.0 * self.1, self.1 * o.1) }
    fn mul(self, o: Rat) -> Rat { Rat::new(self.0 * o.0, self.1 * o.1) }
    fn div(self, o: Rat) -> Rat { Rat::new(self.0 * o.1, self.1 * o.0) }
    fn neg(self) -> Rat { Rat(-self.0, self.1) }
}

fn gcd(a: i64, b: i64) -> i64 { if b == 0 { a } else { gcd(b, a % b) } }

fn extract_targets(line: &str) -> Option<Vec<i64>> {
    let start = line.find('{')?;
    let end = line.find('}')?;
    let targets_str = &line[start + 1..end];
    
    Some(targets_str.split(',')
        .filter_map(|n| n.trim().parse().ok())
        .collect())
}

fn extract_button_vectors(line: &str) -> Vec<Vec<usize>> {
    let mut buttons = Vec::new();
    let mut start = 0;
    
    while let Some(pos) = line[start..].find('(') {
        let abs_start = start + pos;
        if let Some(end) = line[abs_start..].find(')') {
            let abs_end = abs_start + end;
            let button_str = &line[abs_start + 1..abs_end];
            
            let button: Vec<usize> = button_str.split(',')
                .filter_map(|n| n.trim().parse().ok())
                .collect();
            buttons.push(button);
            
            start = abs_end + 1;
        } else {
            break;
        }
    }
    buttons
}

fn solve_part2(line: &str) -> Option<usize> {
    let targets = extract_targets(line)?;
    let buttons = extract_button_vectors(line);
    
    let (m, n) = (targets.len(), buttons.len());
    if n == 0 { return if targets.iter().all(|&t| t == 0) { Some(0) } else { None }; }
    
    let mut mat: Vec<Vec<Rat>> = vec![vec![Rat::zero(); n + 1]; m];
    for (i, &t) in targets.iter().enumerate() { mat[i][n] = Rat::int(t); }
    for (j, btn) in buttons.iter().enumerate() {
        for &ci in btn { if ci < m { mat[ci][j] = Rat::int(1); } }
    }
    
    let mut pivot_col = vec![None; m];
    let mut row = 0;
    for col in 0..n {
        if let Some(pr) = (row..m).find(|&r| !mat[r][col].is_zero()) {
            mat.swap(row, pr);
            pivot_col[row] = Some(col);
            let pivot = mat[row][col];
            for c in 0..=n { mat[row][c] = mat[row][c].div(pivot); }
            for r in 0..m {
                if r != row && !mat[r][col].is_zero() {
                    let f = mat[r][col];
                    for c in 0..=n { mat[r][c] = mat[r][c].sub(f.mul(mat[row][c])); }
                }
            }
            row += 1;
        }
    }
    
    if (row..m).any(|r| !mat[r][n].is_zero()) { return None; }
    
    let pivot_set: std::collections::HashSet<_> = pivot_col.iter().filter_map(|&x| x).collect();
    let free_vars: Vec<_> = (0..n).filter(|c| !pivot_set.contains(c)).collect();
    
    let mut particular = vec![Rat::zero(); n];
    for (r, &pc) in pivot_col.iter().enumerate() {
        if let Some(col) = pc { particular[col] = mat[r][n]; }
    }
    
    let null_basis: Vec<Vec<Rat>> = free_vars.iter().map(|&fv| {
        let mut basis = vec![Rat::zero(); n];
        basis[fv] = Rat::int(1);
        for (r, &pc) in pivot_col.iter().enumerate() {
            if let Some(col) = pc { basis[col] = mat[r][fv].neg(); }
        }
        basis
    }).collect();
    
    if free_vars.is_empty() {
        return particular.iter()
            .try_fold(0i64, |acc, &r| r.to_int().filter(|&v| v >= 0).map(|v| acc + v))
            .map(|t| t as usize);
    }
    
    let bound = *targets.iter().max().unwrap_or(&0);
    let mut best = None;
    search(&mut vec![0; free_vars.len()], 0, bound, &particular, &null_basis, n, &mut best);
    best
}

fn search(fv: &mut Vec<i64>, idx: usize, bound: i64, part: &[Rat], null: &[Vec<Rat>], n: usize, best: &mut Option<usize>) {
    if idx == fv.len() {
        let total: Option<i64> = (0..n).try_fold(0i64, |acc, j| {
            let val = fv.iter().enumerate().fold(part[j], |v, (i, &f)| v.add(null[i][j].mul(Rat::int(f))));
            val.to_int().filter(|&v| v >= 0).map(|v| acc + v)
        });
        if let Some(t) = total {
            if best.map_or(true, |b| (t as usize) < b) { *best = Some(t as usize); }
        }
        return;
    }
    for v in -bound..=bound {
        fv[idx] = v;
        search(fv, idx + 1, bound, part, null, n, best);
    }
}

fn main() -> io::Result<()> {
    let path = std::env::args().nth(1).expect("Provide input file");
    let file = File::open(path)?;
    let lines: Vec<String> = io::BufReader::new(file).lines().collect::<Result<_, _>>()?;
    
    let (mut total1, mut total2) = (0usize, 0usize);
    
    for line in lines.into_iter().map(|s| s.trim().to_string()).filter(|l| !l.is_empty()) {
        if let Some(p) = solve_part1(&line) { total1 += p; }
        if let Some(p) = solve_part2(&line) { total2 += p; }
    }
    
    println!("Part 1: {}", total1);
    println!("Part 2: {}", total2);
    Ok(())
}
