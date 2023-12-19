open System
open System.IO

type Range = { Start: int; End: int }

type Part = { X: Range; M: Range; A: Range; S: Range }

type Field =
    | X
    | M
    | A
    | S

type Operator =
    | Greater
    | Less

type Rule =
    { Field: Field
      Op: Operator
      Value: int
      OnMatch: string }

type Workflow = { Rules: Rule list; Fallback: string }

let trim (text: string) = text.Trim()

let splitBlocks (content: string) =
    match content.Split "\n\n" with
    | [| first; second |] -> Some(first, second)
    | _ -> None

let splitLines (text: string) = text.Split "\n"

let parseRule (rule: string) =
    let field =
        match rule.Chars 0 with
        | 'x' -> X
        | 'm' -> M
        | 'a' -> A
        | 's' -> S
        | _ -> raise (Exception())

    let op =
        match rule.Chars 1 with
        | '>' -> Greater
        | '<' -> Less
        | _ -> raise (Exception())

    let remainder = rule.Substring 2 |> fun x -> x.Split ':'

    let value = remainder[0] |> Int32.Parse
    let onMatch = remainder[1]

    { Field = field
      Op = op
      Value = value
      OnMatch = onMatch }

let parseRules rules =
    rules |> Array.toList |> List.map parseRule

let parseWorkflow (text: string) =
    let tokens = text.Split([| "{"; ","; "}" |], StringSplitOptions.RemoveEmptyEntries)

    tokens[0],
    { Rules = (tokens[1 .. tokens.Length - 2] |> parseRules)
      Fallback = tokens[tokens.Length - 1] }

let addWorkflowToMap (map: Map<string, Workflow>) workflowParse =
    let name, workflow = workflowParse
    map.Add(name, workflow)

let validRange range = range.Start <= range.End

let includeMatch rule range =
    match rule.Op with
    | Greater -> { Start = rule.Value + 1; End = range.End }
    | Less -> { Start = range.Start; End = rule.Value - 1 }

let excludeMatch rule range =
    match rule.Op with
    | Greater -> { Start = range.Start; End = rule.Value }
    | Less -> { Start = rule.Value; End = range.End }

let splitPart matchOp rule part =
    match rule.Field with
    | X -> { part with X = matchOp rule part.X }
    | M -> { part with M = matchOp rule part.M }
    | A -> { part with A = matchOp rule part.A }
    | S -> { part with S = matchOp rule part.S }

let rangeAmount range = uint64(range.End - range.Start + 1)

let partScore part = (rangeAmount part.X) * (rangeAmount part.M) * (rangeAmount part.A) * (rangeAmount part.S)

let rec solve (map: Map<string, Workflow>) stack score =
    match stack with
    | [] -> score
    | head :: rest ->
        let label, part = head

        if not (validRange part.X && validRange part.M && validRange part.A && validRange part.S) then
            solve map rest score
        elif label = "R" then
            solve map rest score
        elif label = "A" then
            solve map rest (score + partScore part)
        else
            let workflow = map[label]
            let mutable remainder = part
            let mutable nextStack = rest

            for rule in workflow.Rules do
                let matchPart = splitPart includeMatch rule remainder
                remainder <- splitPart excludeMatch rule remainder
                nextStack <- (rule.OnMatch, matchPart) :: nextStack
            
            nextStack <- (workflow.Fallback, remainder) :: nextStack

            solve map nextStack score

let content = Environment.GetCommandLineArgs()[2] |> File.ReadAllText |> trim

let workflowsText, _ = content |> splitBlocks |> Option.get

let workflows =
    workflowsText
    |> splitLines
    |> Seq.map parseWorkflow
    |> Seq.fold addWorkflowToMap Map.empty

let startRange = { Start = 1; End = 4000 }
solve workflows ["in", { X = startRange; M = startRange; A = startRange; S = startRange }] 0UL
|> printfn "Part 2: %d"
