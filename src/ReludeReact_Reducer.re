// Heavily inspired by https://github.com/bloodyowl/reason-react-update

// TODO: I'm wondering if we can use some sort of StateT monad or even a WriterT (gasp!) to keep track of
// the side effects emitted by the update actions, rather than the array ref.  I'm not sure if the monad
// will work since things have to return unit at certain key places, so we might be stuck with mutation/ref.

module SideEffect = ReludeReact_SideEffect;

// These are the update commands a component is allowed to emit in the reducer
type update('action, 'state) =
  | NoUpdate
  | Update('state)
  | UpdateWithSideEffect('state, SideEffect.Uncancellable.t('action, 'state))
  | SideEffect(SideEffect.Uncancellable.t('action, 'state))
  | UpdateWithCancellableSideEffect(
      'state,
      SideEffect.Cancellable.t('action, 'state),
    )
  | CancellableSideEffect(SideEffect.Cancellable.t('action, 'state))
  | UpdateWithIO('state, SideEffect.Uncancellable.io('action, 'state))
  | IO(SideEffect.Uncancellable.io('action, 'state));

// A reducer function takes the action and current state and returns an update command
type reducer('action, 'state) = ('action, 'state) => update('action, 'state);

// The react useReducer state stores the actual component state, along with a ref array of
// side effects.  The side effects are collected in the reducer functions, then handled
// using a useEffect hook.  The component should not be, nor need to be aware of the sideEffect business.
type stateAndSideEffects('action, 'state) = {
  state: 'state,
  sideEffects: ref(array(SideEffect.t('action, 'state))),
};

let useReducer = (initialState: 'state, reducer: reducer('action, 'state)) => {
  let ({state, sideEffects}, send) =
    React.useReducer(
      ({state, sideEffects} as stateAndSideEffects, action) => {
        let update = reducer(action, state);

        switch (update) {
        | NoUpdate => stateAndSideEffects

        | Update(state) => {...stateAndSideEffects, state}

        | UpdateWithSideEffect(state, sideEffect) => {
            state,
            sideEffects:
              ref(
                Belt.Array.concat(
                  sideEffects^,
                  [|SideEffect.Uncancellable.lift(sideEffect)|],
                ),
              ),
          }

        | UpdateWithCancellableSideEffect(state, cancellableSideEffect) => {
            state,
            sideEffects:
              ref(
                Belt.Array.concat(
                  sideEffects^,
                  [|SideEffect.Cancellable.lift(cancellableSideEffect)|],
                ),
              ),
          }

        | SideEffect(uncancellableSideEffect) => {
            ...stateAndSideEffects,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|SideEffect.Uncancellable.lift(uncancellableSideEffect)|],
                ),
              ),
          }

        | CancellableSideEffect(cancellableSideEffect) => {
            ...stateAndSideEffects,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|SideEffect.Cancellable.lift(cancellableSideEffect)|],
                ),
              ),
          }

        | UpdateWithIO(state, ioAction) =>
          // The IO must have an 'action type for both the success and error channels - this
          // way we know that the errors have been properly handled and translated to the appropriate action.
          // Run the IO to get the success and error actions, then just send them.
          // TODO: we don't have cancellable IOs (yet?)
          let sideEffect: SideEffect.t('action, 'state) = (
            context => {
              ioAction
              |> Relude.IO.unsafeRunAsync(
                   fun
                   | Ok(action) => context.send(action)
                   | Error(action) => context.send(action),
                 );
              None;
            }
          );
          {
            state,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|sideEffect|],
                ),
              ),
          };

        | IO(ioAction) =>
          let sideEffect: SideEffect.t('action, 'state) = (
            context => {
              ioAction
              |> Relude.IO.unsafeRunAsync(
                   fun
                   | Ok(action) => context.send(action)
                   | Error(action) => context.send(action),
                 );
              None;
            }
          );
          {
            ...stateAndSideEffects,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|sideEffect|],
                ),
              ),
          };
        };
      },
      {state: initialState, sideEffects: ref([||])},
    );

  // This registers the side effects that were emitted by the reducer in a react effect hook.
  // When the hook runs, it will execute all the side effects and will
  // pass along any cancellation functions (bundled in a single cancellation function) to react itself.
  // Each time this runs, the side effects array is cleared.  Also, we pass along the side effects array
  // in the awesome array argument for react to monitor.
  React.useEffect1(
    () =>
      if (Array.length(sideEffects^) > 0) {
        let cancellers: array(SideEffect.canceller) =
          Belt.Array.keepMap(sideEffects^, sideEffect =>
            sideEffect({state, send})
          );

        sideEffects := [||];

        Array.length(cancellers) > 0
          ? Some(() => cancellers->Belt.Array.forEach(func => func())) : None;
      } else {
        None;
      },
    [|sideEffects|],
  );

  // Finally, we return our initial state, and the send function for use in the component
  (state, send);
};