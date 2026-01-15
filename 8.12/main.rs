use std::io::{self, BufRead};
use std::fs::File;
use std::cmp::Ordering;

#[derive(Clone, Copy)]
struct Point {
    x: i64,
    y: i64,
    z: i64,
}

#[derive(Clone, Copy, Eq, PartialEq)]
struct Edge {
    dist2: u64,
    a: usize,
    b: usize,
}

impl Ord for Edge {
    fn cmp(&self, other: &Self) -> Ordering {
        if self.dist2 != other.dist2 {
            return self.dist2.cmp(&other.dist2);
        }
        if self.a != other.a {
            return self.a.cmp(&other.a);
        }
        self.b.cmp(&other.b)
    }
}

impl PartialOrd for Edge {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

struct DSU {
    parent: Vec<usize>,
    rank: Vec<usize>,
}

impl DSU {
    fn new(n: usize) -> Self {
        DSU {
            parent: (0..n).collect(),
            rank: vec![0; n],
        }
    }

    fn find(&mut self, x: usize) -> usize {
        if self.parent[x] != x {
            self.parent[x] = self.find(self.parent[x]);
        }
        self.parent[x]
    }

    fn union(&mut self, x: usize, y: usize) -> bool {
        let px = self.find(x);
        let py = self.find(y);
        if px == py {
            return false;
        }
        if self.rank[px] < self.rank[py] {
            self.parent[px] = py;
        } else if self.rank[px] > self.rank[py] {
            self.parent[py] = px;
        } else {
            self.parent[py] = px;
            self.rank[px] += 1;
        }
        true
    }
}

fn main() -> io::Result<()> {
    let path = std::env::args().nth(1).expect("Provide input file");
    let file = File::open(path)?;
    let reader = io::BufReader::new(file);

    let mut points: Vec<Point> = Vec::new();
    let mut edges: Vec<Edge> = Vec::new();

    for line in reader.lines() {
        let line = line?;
        let line = line.trim();
        if line.is_empty() {
            continue;
        }

        let parts: Vec<&str> = line.split(',').collect();
        if parts.len() != 3 {
            eprintln!("Parse error");
            return Ok(());
        }

        let x: i64 = parts[0].parse().expect("Invalid x");
        let y: i64 = parts[1].parse().expect("Invalid y");
        let z: i64 = parts[2].parse().expect("Invalid z");

        points.push(Point { x, y, z });
    }

    let n = points.len();
    let m = n * (n - 1) / 2;

    let mut part1: i64 = 0;
    let mut part2: i64 = 0;

    if m == 0 {
        part1 = 1;
    }

    // Build edges
    for i in 0..n {
        for j in (i + 1)..n {
            let dx = points[i].x - points[j].x;
            let dy = points[i].y - points[j].y;
            let dz = points[i].z - points[j].z;
            let dist2 = (dx * dx + dy * dy + dz * dz) as u64;

            edges.push(Edge {
                dist2,
                a: i,
                b: j,
            });
        }
    }

    edges.sort();


    let mut dsu = DSU::new(n);
    let to_take = std::cmp::min(1000, m);

    for k in 0..to_take {
        let a = edges[k].a;
        let b = edges[k].b;
        dsu.union(a, b);
    }

    let mut sizes = vec![0; n];
    for i in 0..n {
        let r = dsu.find(i);
        sizes[r] += 1;
    }

    let mut nonzero: Vec<i64> = sizes.iter().map(|&s| s as i64).collect();
    nonzero.sort_by(|a, b| b.cmp(a)); // Sort descending

    let p: i128 = nonzero
        .iter()
        .take(3)
        .fold(1i128, |acc, &v| {
            let v = if v == 0 { 1 } else { v as i128 };
            acc * v
        });

    if p > i64::MAX as i128 {
        println!("part1: {}", p);
    } else {
        part1 = p as i64;
        println!("part1: {}", part1);
    }

   	let mut dsu = DSU::new(n);
    let mut comps2 = n;
    let mut last_a = 0;
    let mut last_b = 0;

    for edge in &edges {
        if dsu.union(edge.a, edge.b) {
            comps2 -= 1;
            if comps2 == 1 {
                last_a = edge.a;
                last_b = edge.b;
                break;
            }
        }
    }

    part2 = points[last_a].x * points[last_b].x;

    let product = (points[last_a].x as i128) * (points[last_b].x as i128);
    if product > i64::MAX as i128 || product < i64::MIN as i128 {
        println!("part2: {}", product);
    } else {
        println!("part2: {}", part2);
    }

    Ok(())
}
