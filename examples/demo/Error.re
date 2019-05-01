type t =
  | NetworkError(Js.Promise.error)
  | DecodeError(Js.Json.t, string);

let show =
  fun
  | NetworkError(_) => "Network error"
  | DecodeError(_) => "Decode error";