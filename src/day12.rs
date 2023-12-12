use std::collections::HashMap;
use std::env;
use std::fs;

fn main() {
    let file = env::args().nth(1).expect("Provide a file name");
    let content = fs::read_to_string(file).expect("Provide a readable file name");

    println!("Part 1: {}", solve(&content, 1));
    println!("Part 2: {}", solve(&content, 5));
}

fn solve(content: &str, multiple: u8) -> usize {
    content.lines().map(|x| get_count(x, multiple)).sum()
}

fn get_count(line: &str, multiple: u8) -> usize {
    let parts: Vec<&str> = line.split(' ').collect();

    let mut row: Vec<char> = parts[0].chars().collect();
    let mut blocks: Vec<usize> = parts[1]
        .split(',')
        .map(|x| x.parse().expect("Only numbers in second form"))
        .collect();
    repeat_data(&mut row, &mut blocks, multiple);

    let mut states = HashMap::new();
    get_count_dynamic(
        &mut states,
        State {
            row_index: 0,
            block_index: 0,
            block_length: 0,
        },
        &row,
        &blocks,
    )
}

fn repeat_data(row: &mut Vec<char>, blocks: &mut Vec<usize>, multiple: u8) {
    let original_row = row.clone();
    let original_blocks = blocks.clone();

    for _ in 1..multiple {
        row.push('?');
        for row_item in original_row.iter() {
            row.push(*row_item);
        }

        for block in original_blocks.iter() {
            blocks.push(*block);
        }
    }
}

// Use dynamic programming to shortcut common outcomes
#[derive(Eq, Hash, PartialEq)]
struct State {
    row_index: usize,
    block_index: usize,
    block_length: usize,
}

fn get_count_dynamic(
    states: &mut HashMap<State, usize>,
    state: State,
    row: &Vec<char>,
    blocks: &Vec<usize>,
) -> usize {
    if states.contains_key(&state) {
        return states[&state];
    }

    if state.row_index == row.len() {
        // finished the row, if the current state is possible -> one option else zero options
        if state.block_index == blocks.len() - 1
            && *blocks.last().expect("No blocks") == state.block_length
        {
            // Regular possible end, blocks are exactly matched
            return 1;
        } else if state.block_index == blocks.len() && state.block_length == 0 {
            // A new block is prepare but no # is found -> newly prepare block does not count
            return 1;
        } else {
            return 0;
        }
    }

    // Recusively determine possible solution count from this point onwards
    let mut count = 0;
    let current = row[state.row_index];

    // Question mark gets all the solutions of a . and a #
    let next_row_index = state.row_index + 1;
    if current == '#' || current == '?' {
        // Block length gets increased for next iteration
        count += get_count_dynamic(
            states,
            State {
                row_index: next_row_index,
                block_length: state.block_length + 1,
                ..state
            },
            row,
            blocks,
        );
    }

    if current == '.' || current == '?' {
        if state.block_length == 0 {
            // Current . does not end a block, it is a consecutive .
            count += get_count_dynamic(
                states,
                State {
                    row_index: next_row_index,
                    ..state
                },
                row,
                blocks,
            );
        } else {
            // Current . ends a block
            // Validate the current block. If its invalid, zero possiblity are found from here
            let is_current_block_valid =
                state.block_index < blocks.len() && blocks[state.block_index] == state.block_length;
            if is_current_block_valid {
                count += get_count_dynamic(
                    states,
                    State {
                        row_index: next_row_index,
                        block_index: state.block_index + 1,
                        block_length: 0,
                    },
                    row,
                    blocks,
                );
            }
        }
    }

    // persist the answer for later shortcuts
    states.insert(state, count);
    count
}
