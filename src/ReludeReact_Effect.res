@ocaml.doc("
 * Registers a callback to run a single time when the component is mounted.
 * This can be used to dispatch an action for handling in a reducer, or for
 * doing any other type of side-effect.
 ")
let useOnMount: (unit => unit) => unit = onMount => React.useEffect1(() => {
    onMount()
    None
  }, [])

@ocaml.doc("
 * Similar to `React.useEffect1`, but requires you to provide an equality
 * function that may determine a prop is equal to a previous prop, even if
 * React's simple default comparison (===) determines the values have changed.
 ")
let useEffect1WithEq: 'a. (unit => unit, ('a, 'a) => bool, 'a) => unit = (f, eq, a) => {
  let refa = React.useRef(None)

  React.useEffect1(() => {
    switch refa.current {
    | Some(old) if eq(old, a) => ()
    | None
    | Some(_) =>
      refa.current = Some(a)
      f()
    }
    None
  }, [a])
}

@ocaml.doc("
 * Similar to `React.useEffect2`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 ")
let useEffect2WithEq: 'a 'b. (
  unit => unit,
  (('a, 'a) => bool, 'a),
  (('b, 'b) => bool, 'b),
) => unit = (f, (eqa, a), (eqb, b)) => {
  let refa = React.useRef(None)
  let refb = React.useRef(None)

  React.useEffect2(() => {
    switch (refa.current, refb.current) {
    | (Some(olda), Some(oldb)) if eqa(olda, a) && eqb(oldb, b) => ()
    | _ =>
      refa.current = Some(a)
      refb.current = Some(b)
      f()
    }
    None
  }, (a, b))
}

@ocaml.doc("
 * Similar to `React.useEffect3`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 ")
let useEffect3WithEq: 'a 'b 'c. (
  unit => unit,
  (('a, 'a) => bool, 'a),
  (('b, 'b) => bool, 'b),
  (('c, 'c) => bool, 'c),
) => unit = (f, (eqa, a), (eqb, b), (eqc, c)) => {
  let refa = React.useRef(None)
  let refb = React.useRef(None)
  let refc = React.useRef(None)

  React.useEffect3(() => {
    switch (refa.current, refb.current, refc.current) {
    | (Some(olda), Some(oldb), Some(oldc)) if eqa(olda, a) && (eqb(oldb, b) && eqc(oldc, c)) => ()
    | _ =>
      refa.current = Some(a)
      refb.current = Some(b)
      refc.current = Some(c)
      f()
    }
    None
  }, (a, b, c))
}

@ocaml.doc("
 * Similar to `React.useEffect4`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 ")
let useEffect4WithEq: 'a 'b 'c 'd. (
  unit => unit,
  (('a, 'a) => bool, 'a),
  (('b, 'b) => bool, 'b),
  (('c, 'c) => bool, 'c),
  (('d, 'd) => bool, 'd),
) => unit = (f, (eqa, a), (eqb, b), (eqc, c), (eqd, d)) => {
  let refa = React.useRef(None)
  let refb = React.useRef(None)
  let refc = React.useRef(None)
  let refd = React.useRef(None)

  React.useEffect4(() => {
    switch (refa.current, refb.current, refc.current, refd.current) {
    | (Some(olda), Some(oldb), Some(oldc), Some(oldd))
      if eqa(olda, a) && (eqb(oldb, b) && (eqc(oldc, c) && eqd(oldd, d))) => ()
    | _ =>
      refa.current = Some(a)
      refb.current = Some(b)
      refc.current = Some(c)
      refd.current = Some(d)
      f()
    }
    None
  }, (a, b, c, d))
}

@ocaml.doc("
 * Similar to `React.useEffect5`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 ")
let useEffect5WithEq: 'a 'b 'c 'd 'e. (
  unit => unit,
  (('a, 'a) => bool, 'a),
  (('b, 'b) => bool, 'b),
  (('c, 'c) => bool, 'c),
  (('d, 'd) => bool, 'd),
  (('e, 'e) => bool, 'e),
) => unit = (f, (eqa, a), (eqb, b), (eqc, c), (eqd, d), (eqe, e)) => {
  let refa = React.useRef(None)
  let refb = React.useRef(None)
  let refc = React.useRef(None)
  let refd = React.useRef(None)
  let refe = React.useRef(None)

  React.useEffect5(() => {
    switch (refa.current, refb.current, refc.current, refd.current, refe.current) {
    | (Some(olda), Some(oldb), Some(oldc), Some(oldd), Some(olde))
      if eqa(olda, a) && (eqb(oldb, b) && (eqc(oldc, c) && (eqd(oldd, d) && eqe(olde, e)))) => ()
    | _ =>
      refa.current = Some(a)
      refb.current = Some(b)
      refc.current = Some(c)
      refd.current = Some(d)
      refe.current = Some(e)
      f()
    }
    None
  }, (a, b, c, d, e))
}

@ocaml.doc("
 * Registers an IO action to run once when the component is mounted, and a callback to handle
 * the final Result value.
 ")
let useIOOnMountWithResult: 'a 'e. (Relude.IO.t<'a, 'e>, Belt.Result.t<'a, 'e> => unit) => unit = (
  io,
  onDone,
) => React.useEffect1(() => {
    io |> Relude.IO.unsafeRunAsync(onDone)
    None
  }, [])

@ocaml.doc("
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final ok value, ignoring any errors that might occur.
 ")
let useIOOnMountWithOk: 'a 'e. (Relude.IO.t<'a, 'e>, 'a => unit) => unit = (io, onOk) =>
  React.useEffect1(() => {
    io |> Relude.IO.unsafeRunAsync(x =>
      switch x {
      | Ok(a) => onOk(a)
      | Error(_) => ()
      }
    )
    None
  }, [])

@ocaml.doc("
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final error value, ignoring any successful value that might be produced.
 ")
let useIOOnMountWithError: 'a 'e. (Relude.IO.t<'a, 'e>, 'e => unit) => unit = (io, onError) =>
  React.useEffect1(() => {
    io |> Relude.IO.unsafeRunAsync(x =>
      switch x {
      | Ok(_) => ()
      | Error(e) => onError(e)
      }
    )
    None
  }, [])

@ocaml.doc("
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final ok or error value.
 ")
let useIOOnMount: 'a 'e. (Relude.IO.t<'a, 'e>, 'a => unit, 'e => unit) => unit = (
  io,
  onOk,
  onError,
) => React.useEffect1(() => {
    io |> Relude.IO.unsafeRunAsync(x =>
      switch x {
      | Ok(a) => onOk(a)
      | Error(e) => onError(e)
      }
    )
    None
  }, [])

module UseAsyncResult = {
  type action<'a, 'e> =
    | Run
    | RunSuccess('a)
    | RunFailure('e)
    | RunAgain

  type state<'a, 'e> = Relude.AsyncResult.t<'a, 'e>

  type send<'a, 'e> = action<'a, 'e> => unit

  let fromIO: (
    Relude.IO.t<'a, 'e>,
    ('b, 'b) => bool,
    'b,
  ) => (Relude.AsyncResult.t<'a, 'e>, unit => unit) = (io_, eq_, key) => {
    let reducer = {
      open ReludeReact_Reducer
      (state, action) =>
        switch action {
        | Run =>
          UpdateWithIO(
            state |> Relude.AsyncResult.toBusy,
            Relude.IO.bimap(ok => RunSuccess(ok), error => RunFailure(error), io_),
          )
        | RunSuccess(ok) => ReludeReact_Reducer.Update(ok |> Relude.AsyncResult.ok)
        | RunFailure(error) => ReludeReact_Reducer.Update(error |> Relude.AsyncResult.error)
        | RunAgain =>
          UpdateWithIO(
            state |> Relude.AsyncResult.foldByValue(
              Relude.AsyncResult.init,
              Relude.AsyncResult.reloadingOk,
              Relude.AsyncResult.reloadingError,
            ),
            Relude.IO.bimap(ok => RunSuccess(ok), error => RunFailure(error), io_),
          )
        }
    }

    let (state, send) = ReludeReact_Reducer.useReducer(reducer, Relude.AsyncResult.init)

    useEffect1WithEq(() => send(Run), eq_, key)

    let again = () => send(RunAgain)

    (state, again)
  }
}
