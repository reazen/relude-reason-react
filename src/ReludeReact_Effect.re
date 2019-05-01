// Shortcut for registering a hook to run at component mount time
let useOnMount: (unit => unit) => unit =
  onMount =>
    React.useEffect1(
      () => {
        onMount();
        None;
      },
      [||],
    );