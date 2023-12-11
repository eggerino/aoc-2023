use std::env;
use std::fs;

fn main() {
    let file = env::args().nth(1).expect("Provide a file name");
    let content = fs::read_to_string(file).expect("Provide a readable file name");

    let image: Vec<Vec<_>> = content.lines().map(|x| x.chars().collect()).collect();

    let galaxies = find_galaxies(&image);
    let empty_rows = find_empty_rows(&image);
    let empty_cols = find_empty_cols(&image);
    let mut part1 = 0;
    let mut part2 = 0;
    for start_galaxy in 0..galaxies.len() {
        for end_galaxy in (start_galaxy + 1)..galaxies.len() {
            part1 += shortest_path(
                &galaxies[start_galaxy],
                &galaxies[end_galaxy],
                &empty_rows,
                &empty_cols,
                2,
            );
            part2 += shortest_path(
                &galaxies[start_galaxy],
                &galaxies[end_galaxy],
                &empty_rows,
                &empty_cols,
                1000000,
            );
        }
    }

    println!("Part 1: {}", part1);
    println!("Part 2: {}", part2);
}

fn find_galaxies(image: &Vec<Vec<char>>) -> Vec<(usize, usize)> {
    let mut galaxies = Vec::new();
    for row in 0..image.len() {
        for col in 0..image[row].len() {
            if image[row][col] == '#' {
                galaxies.push((row, col));
            }
        }
    }
    galaxies
}

fn find_empty_rows(image: &Vec<Vec<char>>) -> Vec<usize> {
    let rows = image.len();
    let mut empty_rows = vec![0; rows];
    let mut empty_count = 0;

    for (i, line) in image.iter().enumerate() {
        if line.iter().all(|x| *x == '.') {
            empty_count += 1;
        }
        empty_rows[i] = empty_count;
    }

    empty_rows
}

fn find_empty_cols(image: &Vec<Vec<char>>) -> Vec<usize> {
    let cols = image.first().expect("No data in image").len();
    let mut empty_cols = vec![0; cols];
    let mut empty_count = 0;

    for i in 0..cols {
        if image.iter().all(|x| x[i] == '.') {
            empty_count += 1;
        }
        empty_cols[i] = empty_count;
    }

    empty_cols
}

fn shortest_path(
    start: &(usize, usize),
    end: &(usize, usize),
    empty_rows: &Vec<usize>,
    empty_cols: &Vec<usize>,
    expansion: usize,
) -> usize {
    let (start_row, start_col) = *start;
    let (end_row, end_col) = *end;

    get_step_count(start_row, end_row, empty_rows, expansion)
        + get_step_count(start_col, end_col, empty_cols, expansion)
}

fn get_step_count(start: usize, end: usize, empty: &Vec<usize>, expansion: usize) -> usize {
    let (lower, upper) = if start < end {
        (start, end)
    } else {
        (end, start)
    };

    let empties = empty[upper] - empty[lower];
    upper - lower + (expansion - 1) * empties
}
