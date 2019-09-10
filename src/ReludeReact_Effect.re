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