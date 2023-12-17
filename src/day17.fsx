open System
open System.IO
open System.Collections.Generic

type Direction =
    | Up
    | Right
    | Down
    | Left

type Position = { Row: int; Col: int }

type MotionState =
    { Pos: Position
      Dir: Direction
      StepCount: int }

type TraversalState = { Motion: MotionState; HeatLoss: int }

let charToInt c = int c - int '0'

let parseLine line = line |> Seq.map charToInt |> Seq.toList

let parseMap content =
    content |> Seq.map parseLine |> Seq.toList

let turnLeft dir =
    match dir with
    | Up -> Left
    | Left -> Down
    | Down -> Right
    | Right -> Up

let turnRight dir =
    match dir with
    | Up -> Right
    | Right -> Down
    | Down -> Left
    | Left -> Up

let move dir pos =
    match dir with
    | Up -> { pos with Row = pos.Row - 1 }
    | Down -> { pos with Row = pos.Row + 1 }
    | Left -> { pos with Col = pos.Col - 1 }
    | Right -> { pos with Col = pos.Col + 1 }

let onMap (map: 'a list list) pos =
    0 <= pos.Row && pos.Row < map.Length && 0 <= pos.Col && pos.Col < map[0].Length

let heatLoss map pos =
    if onMap map pos then map[pos.Row][pos.Col] else 0

let moveState map dir state =
    let pos = move dir state.Motion.Pos

    { Motion =
        { Pos = pos
          Dir = dir
          StepCount =
            if dir = state.Motion.Dir then
                state.Motion.StepCount + 1
            else
                1 }
      HeatLoss = state.HeatLoss + heatLoss map pos }

let enqueueMoveState (queue: PriorityQueue<TraversalState, int>) map dir state =
    let newState = moveState map dir state

    if onMap map newState.Motion.Pos then
        queue.Enqueue(newState, newState.HeatLoss)

let rec recFindSmallestPathHeatLoss
    (map: int list list)
    minSteps
    maxSteps
    (queue: PriorityQueue<TraversalState, int>)
    (seen: MotionState Set)
    =
    if queue.Count = 0 then
        None
    else
        let rows = map.Length
        let cols = map[0].Length
        let state = queue.Dequeue()

        if
            state.Motion.Pos.Row = rows - 1
            && state.Motion.Pos.Col = cols - 1
            && minSteps <= state.Motion.StepCount
        then
            Some(state.HeatLoss)
        elif seen.Contains state.Motion then
            recFindSmallestPathHeatLoss map minSteps maxSteps queue seen
        else

            if minSteps <= state.Motion.StepCount then
                enqueueMoveState queue map (turnLeft state.Motion.Dir) state
                enqueueMoveState queue map (turnRight state.Motion.Dir) state

            if state.Motion.StepCount < maxSteps then
                enqueueMoveState queue map state.Motion.Dir state

            recFindSmallestPathHeatLoss map minSteps maxSteps queue (seen.Add(state.Motion))

let findSmallestPathHeatLoss map minSteps maxSteps startStates =
    let queue = new PriorityQueue<TraversalState, int>()

    for stateState in startStates do
        queue.Enqueue(stateState, stateState.HeatLoss)

    recFindSmallestPathHeatLoss map minSteps maxSteps queue Set.empty

let getStartStates () =
    [ { Motion =
          { Pos = { Row = 0; Col = 0 }
            Dir = Right
            StepCount = 0 }
        HeatLoss = 0 }
      { Motion =
          { Pos = { Row = 0; Col = 0 }
            Dir = Down
            StepCount = 0 }
        HeatLoss = 0 } ]

let map = Environment.GetCommandLineArgs()[2] |> File.ReadAllLines |> parseMap
let startStates = getStartStates ()
printfn "Part 1: %d" (findSmallestPathHeatLoss map 0 3 startStates).Value
printfn "Part 2: %d" (findSmallestPathHeatLoss map 4 10 startStates).Value
