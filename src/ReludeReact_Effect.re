/**
 * Registers a callback to run a single time when the component is mounted.
 * This can be used to dispatch an action for handling in a reducer, or for
 * doing any other type of side-effect.
 */
let useOnMount: (unit => unit) => unit =
  onMount =>
    React.useEffect1(
      () => {
        onMount();
        None;
      },
      [||],
    );

/**
 * Similar to `React.useEffect1`, but requires you to provide an equality
 * function that may determine a prop is equal to a previous prop, even if
 * React's simple default comparison (===) determines the values have changed.
 */
let useEffect1WithEq: 'a. (unit => unit, ('a, 'a) => bool, 'a) => unit =
  (f, eq, a) => {
    let refa = React.useRef(None);

    React.useEffect1(
      () => {
        switch (React.Ref.current(refa)) {
        | Some(old) when eq(old, a) => ()
        | None
        | Some(_) =>
          React.Ref.setCurrent(refa, Some(a));
          f();
        };
        None;
      },
      [|a|],
    );
  };

/**
 * Similar to `React.useEffect2`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 */
let useEffect2WithEq:
  'a 'b.
  (unit => unit, (('a, 'a) => bool, 'a), (('b, 'b) => bool, 'b)) => unit
 =
  (f, (eqa, a), (eqb, b)) => {
    let refa = React.useRef(None);
    let refb = React.useRef(None);

    React.useEffect2(
      () => {
        switch (React.Ref.current(refa), React.Ref.current(refb)) {
        | (Some(olda), Some(oldb)) when eqa(olda, a) && eqb(oldb, b) => ()
        | _ =>
          React.Ref.setCurrent(refa, Some(a));
          React.Ref.setCurrent(refb, Some(b));
          f();
        };
        None;
      },
      (a, b),
    );
  };

/**
 * Similar to `React.useEffect3`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 */
let useEffect3WithEq:
  'a 'b 'c.
  (
    unit => unit,
    (('a, 'a) => bool, 'a),
    (('b, 'b) => bool, 'b),
    (('c, 'c) => bool, 'c)
  ) =>
  unit
 =
  (f, (eqa, a), (eqb, b), (eqc, c)) => {
    let refa = React.useRef(None);
    let refb = React.useRef(None);
    let refc = React.useRef(None);

    React.useEffect3(
      () => {
        switch (
          React.Ref.current(refa),
          React.Ref.current(refb),
          React.Ref.current(refc),
        ) {
        | (Some(olda), Some(oldb), Some(oldc))
            when eqa(olda, a) && eqb(oldb, b) && eqc(oldc, c) =>
          ()
        | _ =>
          React.Ref.setCurrent(refa, Some(a));
          React.Ref.setCurrent(refb, Some(b));
          React.Ref.setCurrent(refc, Some(c));
          f();
        };
        None;
      },
      (a, b, c),
    );
  };

/**
 * Similar to `React.useEffect4`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 */
let useEffect4WithEq:
  'a 'b 'c 'd.
  (
    unit => unit,
    (('a, 'a) => bool, 'a),
    (('b, 'b) => bool, 'b),
    (('c, 'c) => bool, 'c),
    (('d, 'd) => bool, 'd)
  ) =>
  unit
 =
  (f, (eqa, a), (eqb, b), (eqc, c), (eqd, d)) => {
    let refa = React.useRef(None);
    let refb = React.useRef(None);
    let refc = React.useRef(None);
    let refd = React.useRef(None);

    React.useEffect4(
      () => {
        switch (
          React.Ref.current(refa),
          React.Ref.current(refb),
          React.Ref.current(refc),
          React.Ref.current(refd),
        ) {
        | (Some(olda), Some(oldb), Some(oldc), Some(oldd))
            when
              eqa(olda, a) && eqb(oldb, b) && eqc(oldc, c) && eqd(oldd, d) =>
          ()
        | _ =>
          React.Ref.setCurrent(refa, Some(a));
          React.Ref.setCurrent(refb, Some(b));
          React.Ref.setCurrent(refc, Some(c));
          React.Ref.setCurrent(refd, Some(d));
          f();
        };
        None;
      },
      (a, b, c, d),
    );
  };

/**
 * Similar to `React.useEffect5`, but requires you to provide equality functions
 * that may determine props are equal to previous props, even if React's simple
 * default comparison (===) determines the values have changed.
 */
let useEffect5WithEq:
  'a 'b 'c 'd 'e.
  (
    unit => unit,
    (('a, 'a) => bool, 'a),
    (('b, 'b) => bool, 'b),
    (('c, 'c) => bool, 'c),
    (('d, 'd) => bool, 'd),
    (('e, 'e) => bool, 'e)
  ) =>
  unit
 =
  (f, (eqa, a), (eqb, b), (eqc, c), (eqd, d), (eqe, e)) => {
    let refa = React.useRef(None);
    let refb = React.useRef(None);
    let refc = React.useRef(None);
    let refd = React.useRef(None);
    let refe = React.useRef(None);

    React.useEffect5(
      () => {
        switch (
          React.Ref.current(refa),
          React.Ref.current(refb),
          React.Ref.current(refc),
          React.Ref.current(refd),
          React.Ref.current(refe),
        ) {
        | (Some(olda), Some(oldb), Some(oldc), Some(oldd), Some(olde))
            when
              eqa(olda, a)
              && eqb(oldb, b)
              && eqc(oldc, c)
              && eqd(oldd, d)
              && eqe(olde, e) =>
          ()
        | _ =>
          React.Ref.setCurrent(refa, Some(a));
          React.Ref.setCurrent(refb, Some(b));
          React.Ref.setCurrent(refc, Some(c));
          React.Ref.setCurrent(refd, Some(d));
          React.Ref.setCurrent(refe, Some(e));
          f();
        };
        None;
      },
      (a, b, c, d, e),
    );
  };

/**
 * Registers an IO action to run once when the component is mounted, and a callback to handle
 * the final Result value.
 */
let useIOOnMountWithResult:
  'a 'e.
  (Relude.IO.t('a, 'e), Belt.Result.t('a, 'e) => unit) => unit
 =
  (io, onDone) =>
    React.useEffect1(
      () => {
        io |> Relude.IO.unsafeRunAsync(onDone);
        None;
      },
      [||],
    );

/**
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final ok value, ignoring any errors that might occur.
 */
let useIOOnMountWithOk: 'a 'e. (Relude.IO.t('a, 'e), 'a => unit) => unit =
  (io, onOk) =>
    React.useEffect1(
      () => {
        io
        |> Relude.IO.unsafeRunAsync(
             fun
             | Ok(a) => onOk(a)
             | Error(_) => (),
           );
        None;
      },
      [||],
    );

/**
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final error value, ignoring any successful value that might be produced.
 */
let useIOOnMountWithError: 'a 'e. (Relude.IO.t('a, 'e), 'e => unit) => unit =
  (io, onError) =>
    React.useEffect1(
      () => {
        io
        |> Relude.IO.unsafeRunAsync(
             fun
             | Ok(_) => ()
             | Error(e) => onError(e),
           );
        None;
      },
      [||],
    );

/**
 * Registers an IO action to run once when the component is mounted, and separate callbacks to handle
 * the final ok or error value.
 */
let useIOOnMount: 'a 'e. (Relude.IO.t('a, 'e), 'a => unit, 'e => unit) => unit =
  (io, onOk, onError) =>
    React.useEffect1(
      () => {
        io
        |> Relude.IO.unsafeRunAsync(
             fun
             | Ok(a) => onOk(a)
             | Error(e) => onError(e),
           );
        None;
      },
      [||],
    );
