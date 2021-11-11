module Type = {
  type t =
    | Cat
    | Dog
    | Fish
}

type t = {
  animalType: Type.t,
  name: string,
  age: int,
}
