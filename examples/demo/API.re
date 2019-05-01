let animalMutableMap: Belt.HashMap.String.t(Animal.t) =
  Belt.HashMap.String.make(~hintSize=0);

let animalList: unit => list(Animal.t) =
  () => {
    Belt.HashMap.String.toArray(animalMutableMap)
    |> Relude.Array.map(tup => snd(tup))
    |> Relude.List.fromArray;
  };

let fetchAnimals: Relude.IO.t(list(Animal.t), Error.t) =
  Relude.IO.pure(animalList());