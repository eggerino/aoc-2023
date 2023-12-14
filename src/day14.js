const fs = require("fs");

function parsePlatform(content) {
    return content.split("\n").map(x => x.split(""));
}

function toString(platform) {
    return platform.map(x => x.join("")).join("\n")
}

function moveRock(primary, secondary, access, setter, nextPrimary) {
    setter(primary, secondary, ".");
    while (true) {
        const next = nextPrimary(primary);
        if (next === null || access(next, secondary) !== ".") {
            setter(primary, secondary, "O");
            return;
        }
        primary = next;
    }
}

function tiltNorth(platform) {
    for (let row = 0; row < platform.length; ++row) {
        for (let col = 0; col < platform[0].length; ++col) {
            if (platform[row][col] == "O") {
                moveRock(row, col, (p, s) => platform[p][s], (p, s, x) => platform[p][s] = x, x => x > 0 ? x - 1 : null);
            }
        }
    }
}

function tiltWest(platform) {
    for (let row = 0; row < platform.length; ++row) {
        for (let col = 0; col < platform[0].length; ++col) {
            if (platform[row][col] == "O") {
                moveRock(col, row, (p, s) => platform[s][p], (p, s, x) => platform[s][p] = x, x => x > 0 ? x - 1 : null);
            }
        }
    }
}

function tiltSouth(platform) {
    for (let row = platform.length - 1; row >= 0; --row) {
        for (let col = platform[0].length - 1; col >= 0 ; --col) {
            if (platform[row][col] == "O") {
                moveRock(row, col, (p, s) => platform[p][s], (p, s, x) => platform[p][s] = x, x => x < (platform.length - 1) ? x + 1 : null);
            }
        }
    }
}

function tiltEast(platform) {
    for (let row = platform.length - 1; row >= 0; --row) {
        for (let col = platform[0].length - 1; col >= 0 ; --col) {
            if (platform[row][col] == "O") {
                moveRock(col, row, (p, s) => platform[s][p], (p, s, x) => platform[s][p] = x, x => x < (platform[0].length - 1) ? x + 1 : null);
            }
        }
    }
}

function evalNorthBeamLoad(platform) {
    const rows = platform.length;
    let load = 0;
    for (let row = 0; row < rows; ++row) {
        for (let col = 0; col < platform[0].length; ++col) {
            if (platform[row][col] == "O") {
                load += rows - row;
            }
        }
    }
    return load;
}

const content = fs.readFileSync(process.argv[2], "utf-8").trim();
const platform = parsePlatform(content);

tiltNorth(platform);
console.log("Part 1:", evalNorthBeamLoad(platform));

tiltWest(platform);
tiltSouth(platform);
tiltEast(platform);

let cycle = 1;
let platformKey = toString(platform);
const cycleHistory = new Map();

while (!cycleHistory.has(platformKey)) {
    cycleHistory.set(platformKey, cycle);
    tiltNorth(platform);
    tiltWest(platform);
    tiltSouth(platform);
    tiltEast(platform);
    ++cycle;
    platformKey = toString(platform);
}

const first = cycleHistory.get(platformKey);
const periodSize = cycle - first;
const firstResultOccurance = ((1_000_000_000 - first) % periodSize) + first;

cycleHistory.forEach((c, k, _) => {
    if (c === firstResultOccurance) {
        const resultPlatform = parsePlatform(k);
        console.log("Part 2:", evalNorthBeamLoad(resultPlatform));
    }
});