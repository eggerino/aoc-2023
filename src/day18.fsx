// Huge shoutout to HyperNeutrino who did a fantastic job at explaining a elegant solution to this day's problem.
// His video https://www.youtube.com/watch?v=bGWK76_e-LM heavily inspired my solution,
// since I got stuck on part 2 for ether counting individual fields or failing horribly with my own line wise approach.
// I learnt a ton by exploring the theories mentioned in his video.
// Please check it out.
open System
open System.IO

type Direction =
    | Up
    | Down
    | Right
    | Left

type DigInstruction = { Dir: Direction; Amount: int64 }

type Vector2 = { X: int64; Y: int64 }

type Polygon = { Points: Vector2 list; Length: int64 }

let parseInstructionPart1 (line: string) =
    match line.Split(' ') with
    | [| "U"; x; _ |] when fst (Int32.TryParse x) -> Some { Dir = Up; Amount = Int32.Parse x }
    | [| "D"; x; _ |] when fst (Int32.TryParse x) -> Some { Dir = Down; Amount = Int32.Parse x }
    | [| "R"; x; _ |] when fst (Int32.TryParse x) -> Some { Dir = Right; Amount = Int32.Parse x }
    | [| "L"; x; _ |] when fst (Int32.TryParse x) -> Some { Dir = Left; Amount = Int32.Parse x }
    | _ -> None

let parseAmountHex (hexAmount: string) =
    match Int64.TryParse(hexAmount, Globalization.NumberStyles.HexNumber, null) with
    | (true, amount) -> Some(amount)
    | _ -> None

let parseDirectionHex hexDir =
    match hexDir with
    | "0" -> Some(Right)
    | "1" -> Some(Down)
    | "2" -> Some(Left)
    | "3" -> Some(Up)
    | _ -> None

let parseInstructionHex (hex: string) =
    if hex.Length = 6 then
        match (parseAmountHex (hex.Substring(0, 5))), (parseDirectionHex (hex.Substring(5, 1))) with
        | (Some(amount), Some(dir)) -> Some { Amount = amount; Dir = dir }
        | _ -> None
    else
        None

let parseInstructionPart2 (line: string) =
    match line.Split([| "(#"; ")" |], StringSplitOptions.RemoveEmptyEntries) with
    | [| _; hex |] -> parseInstructionHex hex
    | _ -> None

let vecAdd v1 v2 = { X = v1.X + v2.X; Y = v1.Y + v2.Y }

let vecScale scalar v = { X = scalar * v.X; Y = scalar * v.Y }

let unitVectorAlong dir =
    match dir with
    | Up -> { X = 0; Y = 1 }
    | Down -> { X = 0; Y = -1 }
    | Right -> { X = 1; Y = 0 }
    | Left -> { X = -1; Y = 0 }

let collectInstruction polygon inst =
    let head, tail =
        match polygon.Points with
        | head :: tail -> head, tail
        | [] -> { X = 0; Y = 0 }, []

    { Points =
        (inst.Dir |> unitVectorAlong |> vecScale inst.Amount |> vecAdd head)
        :: (head :: tail)
      Length = polygon.Length + inst.Amount }

let wrapIndex (collection: 'a array) index =
    (index + collection.Length) % collection.Length

let polygonArea polygon =
    // Gauss's area formula
    let pointArray = polygon.Points |> List.toArray

    { 1 .. pointArray.Length }
    |> Seq.map (fun x -> x - 1)
    |> Seq.map (fun i ->
        pointArray[i].X
        * (pointArray[wrapIndex pointArray (i + 1)].Y
           - pointArray[wrapIndex pointArray (i - 1)].Y))
    |> Seq.sum
    |> (fun x -> x / 2L)

let outerPolygonArea polygon =
    1L + polygon.Length / 2L + polygonArea polygon



let lines = Environment.GetCommandLineArgs()[2] |> File.ReadAllLines
let emptyPolygon = { Points = []; Length = 0 }

lines
|> Seq.map parseInstructionPart1
|> Seq.map Option.get
|> Seq.fold collectInstruction emptyPolygon
|> outerPolygonArea
|> printfn "Part 1: %d"

lines
|> Seq.map parseInstructionPart2
|> Seq.map Option.get
|> Seq.fold collectInstruction emptyPolygon
|> outerPolygonArea
|> printfn "Part 2: %d"
