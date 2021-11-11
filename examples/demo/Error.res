type t =
  | NetworkError(Js.Promise.error)
  | DecodeError(Js.Json.t, string)

let show = x =>
  switch x {
  | NetworkError(_) => "Network error"
  | DecodeError(_) => "Decode error"
  }
