const fs = require("fs");

function hashAlgorithm(data) {
    let value = 0;
    for (const char of data) {
        value += char.charCodeAt(0);
        value *= 17;
        value %= 256;
    }
    return value;
}

function parseStep(step) {
    if (step.includes("=")) {
        const tokens = step.split("=");
        return {
            label: tokens[0],
            operation: "=",
            focalLength: parseInt(tokens[1]),
        };
    } else {
        return {
            label: step.split("-")[0],
            operation: "-"
        };
    }
}

function insertLens(box, { label, focalLength }) {
    for (let i = 0; i < box.length; ++i) {
        if (box[i].label === label) {
            box[i].focalLength = focalLength;
            return;
        }
    }
    box.push({label, focalLength});
}

function removeLens(box, { label }) {
    for (let i = 0; i < box.length; ++i) {
        if (box[i].label === label) {
            box.splice(i, 1);
            return;
        }
    }
}

const content = fs.readFileSync(process.argv[2], "utf-8").trim();
const steps = content.split(",");
const part1 = steps.map(hashAlgorithm).reduce((acc, x) => acc + x, 0);
console.log("Part 1:", part1);

const boxes = Array(256).fill(undefined);
steps.map(parseStep).forEach(step => {
    const hash = hashAlgorithm(step.label);
    boxes[hash] = boxes[hash] || [];

    if (step.operation === "=") {
        insertLens(boxes[hash], step);
    } else {
        removeLens(boxes[hash], step);
    }
});

let part2 = 0;
for (let iBox = 0; iBox < boxes.length; ++iBox) {
    if (!boxes[iBox]) {
        continue;
    }

    for (let iLens = 0; iLens < boxes[iBox].length; ++iLens) {
        part2 += (iBox + 1) * (iLens + 1) * boxes[iBox][iLens].focalLength;
    }
}
console.log("Part 2:", part2);