use std::collections::HashSet;
use std::env;
use std::fs;

struct Map {
    data: Vec<Vec<char>>,
}

impl Map {
    fn parse(content: &str) -> Self {
        Self {
            data: content.lines().map(|line| line.chars().collect()).collect(),
        }
    }

    fn tile_at(&self, row: usize, col: usize) -> char {
        self.data[row][col]
    }

    fn find_first_tile(&self, tile: char) -> Option<(usize, usize)> {
        for row in 0..self.data.len() {
            for col in 0..self.data[row].len() {
                if self.data[row][col] == tile {
                    return Some((row, col));
                }
            }
        }
        None
    }

    fn get_next_tile_from_start(
        &self,
        row: usize,
        col: usize,
    ) -> Option<(usize, usize, Direction)> {
        if row > 0 && self.data[row - 1][col].can_go_down() {
            Some((row - 1, col, Direction::Up))
        } else if row < self.data.len() - 1 && self.data[row + 1][col].can_go_up() {
            Some((row + 1, col, Direction::Down))
        } else if col > 0 && self.data[row][col - 1].can_go_right() {
            Some((row, col - 1, Direction::Left))
        } else if col < self.data[row].len() - 1 && self.data[row][col + 1].can_go_left() {
            Some((row, col + 1, Direction::Right))
        } else {
            None
        }
    }

    fn get_next_tile(
        &self,
        row: usize,
        col: usize,
        dir: Direction,
    ) -> Option<(usize, usize, Direction)> {
        let current = self.data[row][col];

        if dir != Direction::Down && current.can_go_up() {
            Some((row - 1, col, Direction::Up))
        } else if dir != Direction::Up && current.can_go_down() {
            Some((row + 1, col, Direction::Down))
        } else if dir != Direction::Right && current.can_go_left() {
            Some((row, col - 1, Direction::Left))
        } else if dir != Direction::Left && current.can_go_right() {
            Some((row, col + 1, Direction::Right))
        } else {
            None
        }
    }

    fn replace_none_loop_tiles(&mut self, loop_tiles: &HashSet<(usize, usize)>, tile: char) {
        for row in 0..self.data.len() {
            for col in 0..self.data[row].len() {
                if !loop_tiles.contains(&(row, col)) {
                    self.data[row][col] = tile;
                }
            }
        }
    }

    fn get_side_tiles(
        &self,
        to_left_side: bool,
        row: usize,
        col: usize,
        dir: Direction,
    ) -> Vec<(usize, usize)> {
        let current = self.data[row][col];

        if current == '|' {
            if (dir == Direction::Up && to_left_side) || (dir == Direction::Down && !to_left_side) {
                vec![(row, col - 1)]
            } else {
                vec![(row, col + 1)]
            }
        } else if current == '-' {
            if (dir == Direction::Right && to_left_side)
                || (dir == Direction::Left && !to_left_side)
            {
                vec![(row - 1, col)]
            } else {
                vec![(row + 1, col)]
            }
        } else if current == 'L' {
            if (dir == Direction::Left && to_left_side) || (dir == Direction::Down && !to_left_side)
            {
                vec![(row, col - 1), (row + 1, col)]
            } else {
                Vec::new()
            }
        } else if current == 'F' {
            if (dir == Direction::Left && !to_left_side) || (dir == Direction::Up && to_left_side) {
                vec![(row, col - 1), (row - 1, col)]
            } else {
                Vec::new()
            }
        } else if current == 'J' {
            if (dir == Direction::Right && !to_left_side)
                || (dir == Direction::Down && to_left_side)
            {
                vec![(row, col + 1), (row + 1, col)]
            } else {
                Vec::new()
            }
        } else if current == '7' {
            if (dir == Direction::Right && to_left_side) || (dir == Direction::Up && !to_left_side)
            {
                vec![(row, col + 1), (row - 1, col)]
            } else {
                Vec::new()
            }
        } else {
            Vec::new()
        }
    }

    fn mark_blob(&mut self, row: usize, col: usize, old_tile: char, new_tile: char) {
        if self.data[row][col] != old_tile {
            return;
        }

        self.data[row][col] = new_tile;
        self.mark_blob(row - 1, col, old_tile, new_tile);
        self.mark_blob(row + 1, col, old_tile, new_tile);
        self.mark_blob(row, col - 1, old_tile, new_tile);
        self.mark_blob(row, col + 1, old_tile, new_tile);
    }

    fn count_tiles(&self, tile: char) -> usize {
        self.data.iter().flatten().filter(|x| **x == tile).count()
    }
}

#[derive(Debug, PartialEq, Clone)]
enum Direction {
    Up,
    Down,
    Left,
    Right,
}

impl Direction {
    fn is_left_turn(&self, next: &Self) -> bool {
        match self {
            Direction::Up => *next == Direction::Left,
            Direction::Left => *next == Direction::Down,
            Direction::Down => *next == Direction::Right,
            Direction::Right => *next == Direction::Up,
        }
    }

    fn is_right_turn(&self, next: &Self) -> bool {
        match self {
            Direction::Up => *next == Direction::Right,
            Direction::Right => *next == Direction::Down,
            Direction::Down => *next == Direction::Left,
            Direction::Left => *next == Direction::Up,
        }
    }
}

trait Tile {
    fn can_go_up(&self) -> bool;
    fn can_go_down(&self) -> bool;
    fn can_go_left(&self) -> bool;
    fn can_go_right(&self) -> bool;
}

impl Tile for char {
    fn can_go_up(&self) -> bool {
        match self {
            '|' | 'L' | 'J' => true,
            _ => false,
        }
    }

    fn can_go_down(&self) -> bool {
        match self {
            '|' | '7' | 'F' => true,
            _ => false,
        }
    }
    fn can_go_left(&self) -> bool {
        match self {
            '-' | 'J' | '7' => true,
            _ => false,
        }
    }
    fn can_go_right(&self) -> bool {
        match self {
            '-' | 'L' | 'F' => true,
            _ => false,
        }
    }
}

fn main() {
    let file = env::args().nth(1).expect("Provide a file name");
    let content = fs::read_to_string(file).expect("Provide a readable file name");

    let mut map = Map::parse(&content);

    let (start_row, start_col) = map.find_first_tile('S').expect("No S in map");

    let mut left_turn_count = 0;
    let mut loop_tiles = HashSet::new();
    loop_tiles.insert((start_row, start_col));

    let (first_row, first_col, first_dir) = map
        .get_next_tile_from_start(start_row, start_col)
        .expect("S has no connecting tile");
    let (mut row, mut col, mut dir) = (first_row, first_col, first_dir.clone());
    while map.tile_at(row, col) != 'S' {
        loop_tiles.insert((row, col));

        let (new_row, new_col, new_dir) = map
            .get_next_tile(row, col, dir.clone())
            .expect("Tile at row {row} col {col} is a dead end");

        if dir.is_left_turn(&new_dir) {
            left_turn_count += 1;
        } else if dir.is_right_turn(&new_dir) {
            left_turn_count -= 1;
        }

        (row, col, dir) = (new_row, new_col, new_dir);
    }

    map.replace_none_loop_tiles(&loop_tiles, '.');

    let to_left_side = left_turn_count > 0;
    (row, col, dir) = (first_row, first_col, first_dir);
    while map.tile_at(row, col) != 'S' {
        for (side_row, side_col) in map.get_side_tiles(to_left_side, row, col, dir.clone()) {
            map.mark_blob(side_row, side_col, '.', 'I');
        }

        (row, col, dir) = map
            .get_next_tile(row, col, dir)
            .expect("Tile at row {row} col {col} is a dead end");
    }

    println!("Part 1: {}", loop_tiles.len() / 2);
    println!("Part 2: {}", map.count_tiles('I'));
}
