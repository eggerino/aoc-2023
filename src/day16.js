const fs = require("fs");

const content = fs.readFileSync(process.argv[2], "utf-8").trim();
const map = content.split("\n").map(x => x.split(""));

const rows = map.length;
const cols = map[0].length;

const beamStartPart1 = { row: 0, col: 0, dir: 0 };
const beamStartsPart2 = [];
for (let i = 0; i < rows; ++i) {
    beamStartsPart2.push({ row: i, col: 0, dir: 0 })
    beamStartsPart2.push({ row: i, col: cols - 1, dir: 2 })
}
for (let i = 0; i < cols; ++i) {
    beamStartsPart2.push({ row: 0, col: i, dir: 1 })
    beamStartsPart2.push({ row: rows - 1, col: i, dir: 3 })
}

function getEnergizedTileCount(beamStart) {
    const marks = Array(rows).fill(null).map(_ => Array(cols).fill(null).map(_ => []));
    const beamHeads = [beamStart];

    while (beamHeads.length > 0) {
        const beamHead = beamHeads.pop();

        if (beamHead.row < 0 || beamHead.row >= rows || beamHead.col < 0 || beamHead.col >= cols) {
            continue;
        }

        if (marks[beamHead.row][beamHead.col].includes(beamHead.dir)) {
            continue;
        }

        marks[beamHead.row][beamHead.col].push(beamHead.dir);
        const tile = map[beamHead.row][beamHead.col];

        const newDirs = [];
        if (beamHead.dir % 2 === 0) {
            // horizontal direction
            if (tile === "\\" || tile === "|") {
                newDirs.push(beamHead.dir + 1);
            }
            if (tile === "/" || tile === "|") {
                newDirs.push(beamHead.dir - 1);
            }
            if (tile === "." || tile === "-") {
                newDirs.push(beamHead.dir);
            }
        } else {
            // Vertical direction
            if (tile === "/" || tile === "-") {
                newDirs.push(beamHead.dir + 1);
            }
            if (tile === "\\" || tile === "-") {
                newDirs.push(beamHead.dir - 1);
            }
            if (tile === "." || tile === "|") {
                newDirs.push(beamHead.dir);
            }
        }

        newDirs.forEach(dir => {
            beamHeads.push(move(beamHead, dir));
        });
    }

    let tileCount = 0;
    marks.forEach(r => r.forEach(x => {
        if (x.length > 0) {
            ++tileCount;
        }
    }));
    return tileCount;
}

function move({ row, col }, dir) {
    dir = (dir + 4) % 4;
    if (dir % 2 === 0) {
        return { row, col: dir === 0 ? col + 1 : col - 1, dir };
    } else {
        return { row: dir === 1 ? row + 1 : row - 1, col, dir };
    }
}

console.log("Part 1:", getEnergizedTileCount(beamStartPart1));
console.log("Part 2:", Math.max(...beamStartsPart2.map(getEnergizedTileCount)));
