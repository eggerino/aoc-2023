// In this solution is an assumption for part 2, that wasn't specified in the puzzle
// Part 2 only works if EXACTLY one conjunction sends signals to rx
// If the given data set does not have this property, the solution will not work
open System
open System.IO
open System.Collections.Generic

// Types
type ModuleType =
    | UnknownType
    | FlipFlopType
    | ConjunctionType

type LineTokens =
    { Type: ModuleType
      Name: string
      Targets: string array }

type Broadcast = { Targets: string array }

type FlipFlop = { Targets: string array; State: bool }

type Conjunction =
    { Targets: string array
      States: Map<string, bool> }

type Module =
    | Broadcast of Broadcast
    | FlipFlop of FlipFlop
    | Conjunction of Conjunction

type Signal =
    { Source: string
      Target: string
      Pulse: bool }

// Parsing logic
let tokenizeLine (line: string) =
    let moduleType =
        match line.Chars 0 with
        | '%' -> FlipFlopType
        | '&' -> ConjunctionType
        | _ -> UnknownType

    let sourceTargetSplit =
        line.Split([| "%"; "&"; " -> " |], StringSplitOptions.RemoveEmptyEntries)

    { Type = moduleType
      Name = sourceTargetSplit[0]
      Targets = sourceTargetSplit[1].Split(", ") }

let parseBroadcast tokens =
    if tokens.Type = UnknownType then
        Some({ Targets = tokens.Targets })
    else
        None

let parseFlipFlop tokens =
    if tokens.Type = FlipFlopType then
        Some(
            { Targets = tokens.Targets
              State = false }
        )
    else
        None

let parseConjunction (allTokens: LineTokens seq) tokens =
    if tokens.Type = ConjunctionType then
        let states =
            allTokens
            |> Seq.filter (fun x -> (x.Targets |> Array.contains tokens.Name))
            |> Seq.map (fun x -> x.Name)
            |> Seq.fold (fun map source -> Map.add source false map) Map.empty

        Some(
            { Targets = tokens.Targets
              States = states }
        )
    else
        None

let parseModule allTokens tokens =
    match parseBroadcast tokens with
    | Some(x) -> Some(Broadcast(x))
    | None ->
        match parseFlipFlop tokens with
        | Some(x) -> Some(FlipFlop(x))
        | None ->
            match parseConjunction allTokens tokens with
            | Some(x) -> Some(Conjunction(x))
            | None -> None

// General utlilities
let rec first predicate items =
    match items with
    | [] -> None
    | head :: _ when predicate head -> Some(head)
    | _ :: rest -> first predicate rest

let enqueueRange items (queue: Queue<'a>) =
    items |> Seq.iter (fun x -> queue.Enqueue x)

// Math utilities
let rec gcd a b =
    match (a, b) with
    | (_, 0L) -> a
    | (0L, _) -> b
    | (_, _) -> gcd b (a % b)

let lcm a b = a * b / (gcd a b)

// Domain logic
let previousModules moduleName (allTokens: LineTokens seq) =
    allTokens |> Seq.filter (fun t -> t.Targets |> Seq.contains moduleName)

let invokeBroadcast (broadcast: Broadcast) signal =
    broadcast.Targets
    |> Seq.map (fun x ->
        { Source = signal.Target
          Target = x
          Pulse = signal.Pulse })

let invokeFlipFLop flipFlop signal =
    match signal.Pulse with
    | true -> flipFlop, Seq.empty
    | false ->
        { flipFlop with
            State = not flipFlop.State },
        flipFlop.Targets
        |> Seq.map (fun x ->
            { Source = signal.Target
              Target = x
              Pulse = not flipFlop.State })

let invokeConjunction conjunction signal =
    let newConjuction =
        { conjunction with
            States = Map.change signal.Source (fun _ -> Some(signal.Pulse)) conjunction.States }

    let allHigh = newConjuction.States.Values |> Seq.forall (fun x -> x)

    newConjuction,
    conjunction.Targets
    |> Seq.map (fun x ->
        { Source = signal.Target
          Target = x
          Pulse = not allHigh })

let invokeModule module_ signal =
    match module_ with
    | Broadcast(x) -> module_, invokeBroadcast x signal
    | FlipFlop(x) -> (invokeFlipFLop x signal) |> (fun r -> FlipFlop(fst r), snd r)
    | Conjunction(x) -> (invokeConjunction x signal) |> (fun r -> Conjunction(fst r), snd r)

let rec resolveSignals (moduleMap: Map<string, Module>) (signalQueue: Queue<Signal>) signalHistory =
    match signalQueue.TryDequeue() with
    | (false, _) -> moduleMap, signalHistory
    | (true, signal) ->
        match Map.tryFind signal.Target moduleMap with
        | None -> resolveSignals moduleMap signalQueue (signal :: signalHistory)
        | Some(currentModule) ->
            let updatedModule, signals = invokeModule currentModule signal
            enqueueRange signals signalQueue

            resolveSignals
                (Map.change signal.Target (fun _ -> Some(updatedModule)) moduleMap)
                signalQueue
                (signal :: signalHistory)

let countSignals pulse signals =
    signals |> Seq.filter (fun x -> x.Pulse = pulse) |> Seq.length


// Parse
let lines = Environment.GetCommandLineArgs()[2] |> File.ReadAllLines
let allTokens = lines |> Array.map tokenizeLine
let moduleNames = allTokens |> Seq.map (fun x -> x.Name)
let modules = allTokens |> Seq.map (parseModule allTokens) |> Seq.map Option.get

let moduleMap =
    Seq.zip moduleNames modules
    |> Seq.fold (fun m kvp -> Map.add (fst kvp) (snd kvp) m) Map.empty

// Prepare main loop
let signalQueue = Queue<Signal>()
let mutable mutModuleMap = moduleMap
let mutable buttonCount = 0L

// Part 1
let mutable part1Solved = false
let mutable lowCount = 0L
let mutable highCount = 0L

// Part 2
let mutable part2Solved = false
let lastModules = allTokens |> previousModules "rx" |> Seq.toArray

if not (lastModules.Length = 1 && lastModules[0].Type = ConjunctionType) then
    raise (Exception("Assertion: Only one conjunction can signal to 'rx'"))

let lastCon = lastModules[0]

let intoLastConCount =
    allTokens
    |> Seq.filter (fun t -> t.Targets |> Seq.contains lastCon.Name)
    |> Seq.length

let mutable intoConCycleMap = Map.empty

while not (part1Solved && part2Solved) do
    buttonCount <- buttonCount + 1L

    signalQueue.Enqueue
        { Source = "Button"
          Target = "broadcaster"
          Pulse = false }

    let newModuleMap, signalHistory = resolveSignals mutModuleMap signalQueue []

    mutModuleMap <- newModuleMap

    if buttonCount <= 1000 then
        lowCount <- lowCount + (signalHistory |> countSignals false |> int64)
        highCount <- highCount + (signalHistory |> countSignals true |> int64)

    if buttonCount = 1000 then
        part1Solved <- true

    let signal = signalHistory |> first (fun s -> s.Pulse && s.Target = lastCon.Name)

    match signal with
    | None -> ()
    | Some(s) ->
        match Map.containsKey s.Source intoConCycleMap with
        | true -> ()
        | false -> intoConCycleMap <- Map.add s.Source buttonCount intoConCycleMap

    if intoConCycleMap.Count = intoLastConCount then
        part2Solved <- true

printfn "Part 1: %d" (lowCount * highCount)
intoConCycleMap.Values |> Seq.fold lcm 1L |> printfn "Part 2: %d"
