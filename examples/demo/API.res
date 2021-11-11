let animalMutableMap: Belt.HashMap.String.t<Animal.t> = Belt.HashMap.String.make(~hintSize=0)

let animalList: unit => list<Animal.t> = () =>
  Belt.HashMap.String.toArray(animalMutableMap) |> Relude.Array.map(snd) |> Relude.List.fromArray

let artificalDelay = 2000

let fetchAnimals: Relude.IO.t<list<Animal.t>, Error.t> =
  Relude.IO.pure(animalList()) |> Relude.IO.withDelay(artificalDelay)
