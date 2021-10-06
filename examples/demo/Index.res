let root = ReactDOM.querySelector("#root")

let () = switch root {
| Some(element) => ReactDOM.render(<Router />, element)
| None => Js.Exn.raiseError("Root not found!")
}
