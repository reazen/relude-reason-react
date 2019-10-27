// Heavily inspired by https://github.com/bloodyowl/reason-react-update

// TODO: I'm wondering if we can use some sort of StateT monad or even a WriterT (gasp!) to keep track of
// the side effects emitted by the update actions, rather than the array ref.  I'm not sure if the monad
// will work since things have to return unit at certain key places, so we might be stuck with mutation/ref.

module SideEffect = ReludeReact_SideEffect;

// These are the update commands a component is allowed to emit in the reducer
type update('action, 'state) =
  | NoUpdate
  | Update('state)
  | UpdateWithSideEffect('state, SideEffect.Uncancelable.t('action, 'state))
  | SideEffect(SideEffect.Uncancelable.t('action, 'state))
  | UpdateWithCancelableSideEffect(
      'state,
      SideEffect.Cancelable.t('action, 'state),
    )
  | CancelableSideEffect(SideEffect.Cancelable.t('action, 'state))
  | UpdateWithIO('state, SideEffect.Uncancelable.IO.t('action, 'state))
  | IO(SideEffect.Uncancelable.IO.t('action, 'state));

// constructors as functions to make piping easier
let update = state => Update(state);

let updateWithSideEffect = (state, effect) =>
  UpdateWithSideEffect(state, effect);

let sideEffect = effect => SideEffect(effect);

let updateWithCancelableSideEffect = (state, effect) =>
  UpdateWithCancelableSideEffect(state, effect);

let cancelableSideEffect = effect => CancelableSideEffect(effect);

let updateWithIO = (state, io) => UpdateWithIO(state, io);

let io = io => IO(io);

// A reducer function takes the current state and an action and returns an update command
type reducer('state, 'action) = ('state, 'action) => update('action, 'state);

// The react useReducer state stores the actual component state, along with a ref array of
// side effects.  The side effects are collected in the reducer functions, then handled
// using a useEffect hook.  The component should not be, nor need to be aware of the sideEffect business.
type stateAndSideEffects('action, 'state) = {
  state: 'state,
  sideEffects: ref(array(SideEffect.t('action, 'state))),
};

let useReducer = (reducer: reducer('state, 'action), initialState: 'state) => {
  let ({state, sideEffects}, send) =
    React.useReducer(
      ({state, sideEffects} as stateAndSideEffects, action) => {
        let update = reducer(state, action);

        switch (update) {
        | NoUpdate => stateAndSideEffects

        | Update(state) => {...stateAndSideEffects, state}

        | UpdateWithSideEffect(state, sideEffect) => {
            state,
            sideEffects:
              ref(
                Belt.Array.concat(
                  sideEffects^,
                  [|SideEffect.Uncancelable.lift(sideEffect)|],
                ),
              ),
          }

        | UpdateWithCancelableSideEffect(state, cancelableSideEffect) => {
            state,
            sideEffects:
              ref(
                Belt.Array.concat(
                  sideEffects^,
                  [|SideEffect.Cancelable.lift(cancelableSideEffect)|],
                ),
              ),
          }

        | SideEffect(uncancelableSideEffect) => {
            ...stateAndSideEffects,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|SideEffect.Uncancelable.lift(uncancelableSideEffect)|],
                ),
              ),
          }

        | CancelableSideEffect(cancelableSideEffect) => {
            ...stateAndSideEffects,
            sideEffects:
              ref(
                Belt.Array.concat(
                  stateAndSideEffects.sideEffects^,
                  [|SideEffect.Cancelable.lift(cancelableSideEffect)|],
                ),
              ),
          }

        | UpdateWithIO(state, ioAction) =>
          // The IO must have an 'action type for both the success and error channels - this
          // way we know that the errors have been properly handled and translated to the appropriate action.
          // Run the IO to get the success and error actions, then just send them.
          // TODO: we don't have cancelable IOs (yet?)
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
  // pass along any cancelation functions (bundled in a single cancelation function) to react itself.
  // Each time this runs, the side effects array is cleared.  Also, we pass along the side effects array
  // in the awesome array argument for react to monitor.
  React.useEffect1(
    () =>
      if (Array.length(sideEffects^) > 0) {
        let cancelers: array(SideEffect.canceler) =
          Belt.Array.keepMap(sideEffects^, sideEffect =>
            sideEffect({state, send})
          );

        sideEffects := [||];

        Array.length(cancelers) > 0
          ? Some(() => cancelers->Belt.Array.forEach(func => func())) : None;
      } else {
        None;
      },
    [|sideEffects|],
  );

  // Finally, we return our initial state, and the send function for use in the component
  (state, send);
};
