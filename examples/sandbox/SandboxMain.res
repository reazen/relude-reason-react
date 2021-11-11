let root = ReactDOM.querySelector("#root")

let () = switch root {
| Some(element) => ReactDOM.render(<SandboxApp />, element)
| None => Js.Exn.raiseError("Root not found!")
}
