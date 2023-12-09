use std::env;
use std::fs;

fn main() {
    let file = env::args().nth(1).expect("Provide a file name");
    let content = fs::read_to_string(file).expect("Provide a readable file name");

    let (part1, part2) = content
        .lines()
        .map(|line| {
            let values: Vec<_> = line
                .split(" ")
                .map(|x| x.parse().expect("Only numbers allowed in file"))
                .collect();
            predict(&values)
        })
        .fold((0, 0), |(p1, p2), (prev, next)| (p1 + next, p2 + prev));

    println!("Part 1 {}", part1);
    println!("Part 2 {}", part2);
}

fn predict(values: &[i32]) -> (i32, i32) {
    if values.iter().all(|x| *x == 0) {
        return (0, 0);
    }

    let diffs: Vec<_> = values.windows(2).map(|x| x[1] - x[0]).collect();
    let (diff_prev, diff_next) = predict(&diffs);

    return (
        values.first().expect("Unable to reduce to zero list") - diff_prev,
        values.last().expect("Unable to reduce to zero list") + diff_next,
    );
}
