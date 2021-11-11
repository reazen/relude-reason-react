// Heavily inspired by https://github.com/bloodyowl/reason-react-update

// TODO: I'm wondering if we can use some sort of StateT monad or even a WriterT (gasp!) to keep track of
// the side effects emitted by the update actions, rather than the array ref.  I'm not sure if the monad
// will work since things have to return unit at certain key places, so we might be stuck with mutation/ref.

module SideEffect = ReludeReact_SideEffect

// These are the update commands a component is allowed to emit in the reducer
type update<'action, 'state> =
  | NoUpdate
  | Update('state)
  | UpdateWithSideEffect('state, SideEffect.Uncancelable.t<'action, 'state>)
  | SideEffect(SideEffect.Uncancelable.t<'action, 'state>)
  | UpdateWithCancelableSideEffect('state, SideEffect.Cancelable.t<'action, 'state>)
  | CancelableSideEffect(SideEffect.Cancelable.t<'action, 'state>)
  | UpdateWithIO('state, SideEffect.Uncancelable.IO.t<'action, 'state>)
  | IO(SideEffect.Uncancelable.IO.t<'action, 'state>)

// constructors as functions to make piping easier
let update = state => Update(state)

let updateWithSideEffect = (state, effect) => UpdateWithSideEffect(state, effect)

let sideEffect = effect => SideEffect(effect)

let updateWithCancelableSideEffect = (state, effect) => UpdateWithCancelableSideEffect(
  state,
  effect,
)

let cancelableSideEffect = effect => CancelableSideEffect(effect)

let updateWithIO = (state, io) => UpdateWithIO(state, io)

let io = io => IO(io)

// A reducer function takes the current state and an action and returns an update command
type reducer<'action, 'state> = ('state, 'action) => update<'action, 'state>

// This new state type stores the caller's state along with a mutable array of effects that
// need to be run at the appropriate time. The effects are given to us via the `update`
// constructors like SideEffect, UpdateWithIO, IO, etc.
type stateAndSideEffects<'action, 'state> = {
  state: 'state,
  sideEffects: ref<array<SideEffect.t<'action, 'state>>>,
}

@ocaml.doc("
 * Accepts a reducer function that emits `update` commands. Any updates that include side
 * effects (SideEffect, UpdateWithIO, etc.) will be handled by enqueuing the side effects
 * for execution outside of the reducer in a controlled fashion. The side effects are expected
 * to dispatch further reducer actions to cause state changes, etc. IO-based effects are expected
 * to produce actions that will be dispatched automatically.
 ")
let useReducer = (reducer: reducer<'action, 'state>, initialState: 'state) => {
  // This wraps the given reducer function with the ability to capture the side effects
  // emitted by the various types of updates, and stick them in our mutable array of effects to run later
  let reducerWithSideEffects = ({state, sideEffects} as stateAndSideEffects, action) => {
    let update = reducer(state, action)

    switch update {
    | NoUpdate => stateAndSideEffects

    | Update(state) => {...stateAndSideEffects, state: state}

    | UpdateWithSideEffect(state, sideEffect) => {
        state: state,
        sideEffects: ref(
          Belt.Array.concat(sideEffects.contents, [SideEffect.Uncancelable.lift(sideEffect)]),
        ),
      }

    | UpdateWithCancelableSideEffect(state, cancelableSideEffect) => {
        state: state,
        sideEffects: ref(
          Belt.Array.concat(
            sideEffects.contents,
            [SideEffect.Cancelable.lift(cancelableSideEffect)],
          ),
        ),
      }

    | SideEffect(uncancelableSideEffect) => {
        ...stateAndSideEffects,
        sideEffects: ref(
          Belt.Array.concat(
            stateAndSideEffects.sideEffects.contents,
            [SideEffect.Uncancelable.lift(uncancelableSideEffect)],
          ),
        ),
      }

    | CancelableSideEffect(cancelableSideEffect) => {
        ...stateAndSideEffects,
        sideEffects: ref(
          Belt.Array.concat(
            stateAndSideEffects.sideEffects.contents,
            [SideEffect.Cancelable.lift(cancelableSideEffect)],
          ),
        ),
      }

    | UpdateWithIO(state, ioAction) =>
      // The IO must have an 'action type for both the success and error channels - this
      // way we know that the errors have been properly handled and translated to the appropriate action.
      // Run the IO to get the success and error actions, then just send them.
      // TODO: we don't have cancelable IOs (yet?)
      let sideEffect: SideEffect.t<'action, 'state> = context => {
        ioAction |> Relude.IO.unsafeRunAsync(x =>
          switch x {
          | Ok(action) => context.send(action)
          | Error(action) => context.send(action)
          }
        )
        None
      }
      {
        state: state,
        sideEffects: ref(Belt.Array.concat(stateAndSideEffects.sideEffects.contents, [sideEffect])),
      }

    | IO(ioAction) =>
      let sideEffect: SideEffect.t<'action, 'state> = context => {
        ioAction |> Relude.IO.unsafeRunAsync(x =>
          switch x {
          | Ok(action) => context.send(action)
          | Error(action) => context.send(action)
          }
        )
        None
      }
      {
        ...stateAndSideEffects,
        sideEffects: ref(Belt.Array.concat(stateAndSideEffects.sideEffects.contents, [sideEffect])),
      }
    }
  }

  // Our new initial state is the caller's state, plus our initial empty array of effects to run
  let initialStateWithSideEffects = {
    state: initialState,
    sideEffects: ref([]),
  }

  // Plug our new reducer function into the React user reducer. This reducer takes the `update`s from the caller's
  // reducers and enqueues the side effects in a mutable array for processing below in a separate useEffect
  let ({state, sideEffects}, send) = React.useReducer(
    reducerWithSideEffects,
    initialStateWithSideEffects,
  )

  // This registers the side effects that were emitted by the reducer in a react effect hook.
  // When the hook runs, it will execute all the side effects and will
  // pass along any cancelation functions (bundled in a single cancelation function) to react itself.
  // Each time this runs, the side effects array is cleared.  Also, we pass along the side effects array
  // in the awesome array argument for react to monitor.
  React.useEffect1(() =>
    if Array.length(sideEffects.contents) > 0 {
      let cancelers: array<SideEffect.canceler> = Belt.Array.keepMap(
        sideEffects.contents,
        sideEffect => sideEffect({state: state, send: send}),
      )

      sideEffects := []

      Array.length(cancelers) > 0 ? Some(() => cancelers->Belt.Array.forEach(func => func())) : None
    } else {
      None
    }
  , [sideEffects])

  // Finally, we return our initial state, and the send function for use in the calling component
  (state, send)
}
