const fs = require("fs");

const content = fs.readFileSync(process.argv[2], "utf-8").trim();
const patterns = content.split("\n\n").map(parsePattern);

function parsePattern(pattern) {
    return pattern.split("\n").map(l => l.split(""))
}

function isRefelction(corrections, primary, primaryLimit, secondaryLimit, access) {
    let errorCount = 0;

    let lowerPrimary = primary;
    let upperPrimary = primary + 1;

    while (lowerPrimary >= 0 && upperPrimary < primaryLimit) {

        for (let secondary = 0; secondary < secondaryLimit; ++secondary) {
            if (access(lowerPrimary, secondary) != access(upperPrimary, secondary)) {
                ++errorCount;
            }

            if (errorCount > corrections) {
                return false;
            }
        }

        --lowerPrimary;
        ++upperPrimary;
    }

    return errorCount === corrections;
}

function findReflection(corrections, primaryLimit, secondaryLimit, access) {
    for (let primary = 0; primary + 1 < primaryLimit; ++primary) {
        if (isRefelction(corrections, primary, primaryLimit, secondaryLimit, access)) {
            return primary + 1;
        }
    }
    return null;
}

function evalPatternScore(pattern, corrections) {
    const rows = pattern.length;
    const cols = pattern[0].length;

    const vertical = findReflection(corrections, cols, rows, (p, s) => pattern[s][p]);
    if (vertical !== null) {
        return vertical;
    }

    const horizontal = findReflection(corrections, rows, cols, (p, s) => pattern[p][s]);
    if (horizontal !== null) {
        return horizontal * 100;
    }

    return 0;
}

console.log("Part 1:", patterns.map(p => evalPatternScore(p, 0)).reduce((acc, x) => acc + x, 0));
console.log("Part 2:", patterns.map(p => evalPatternScore(p, 1)).reduce((acc, x) => acc + x, 0));
