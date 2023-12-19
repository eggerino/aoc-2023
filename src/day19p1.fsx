open System
open System.IO

type Part = { X: int; M: int; A: int; S: int }

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

let trim (text: string) = text.Trim()

let splitBlocks (content: string) =
    match content.Split "\n\n" with
    | [| first; second |] -> Some(first, second)
    | _ -> None

let splitLines (text: string) = text.Split "\n"

let isNumber (text: string) = text |> Int32.TryParse |> fst

let toNumber (text: string) = text |> Int32.TryParse |> snd

let parsePart (line: string) =
    match line.Split([| "{x="; ",m="; ",a="; ",s="; "}" |], StringSplitOptions.RemoveEmptyEntries) with
    | [| x; m; a; s |] when (isNumber x) && (isNumber m) && (isNumber a) && (isNumber s) ->
        Some(
            { X = toNumber x
              M = toNumber m
              A = toNumber a
              S = toNumber s }
        )
    | _ -> None

let partSum part = part.X + part.M + part.A + part.S

let checkRule rule part =
    let partValue =
        match rule.Field with
        | X -> part.X
        | M -> part.M
        | A -> part.A
        | S -> part.S

    match rule.Op with
    | Greater -> partValue > rule.Value
    | Less -> partValue < rule.Value


let rec applyRules rules fallback part =
    match rules with
    | rule :: _ when checkRule rule part -> rule.OnMatch
    | _ :: rest -> applyRules rest fallback part
    | [] -> fallback

let rec evaluatePart (map: Map<string, Workflow>) workflow part =
    if workflow = "A" then
        true
    else if workflow = "R" then
        false
    else
        let workflowItem = map[workflow]
        let next = applyRules workflowItem.Rules workflowItem.Fallback part
        evaluatePart map next part

let content = Environment.GetCommandLineArgs()[2] |> File.ReadAllText |> trim

let workflowsText, partsText = content |> splitBlocks |> Option.get

let workflows =
    workflowsText
    |> splitLines
    |> Seq.map parseWorkflow
    |> Seq.fold addWorkflowToMap Map.empty


partsText
|> splitLines
|> Seq.map parsePart
|> Seq.map Option.get
|> Seq.filter (evaluatePart workflows "in")
|> Seq.map partSum
|> Seq.sum
|> printfn "Part 1: %d"
