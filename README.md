# relude-reason-react

[![CircleCI](https://img.shields.io/circleci/project/github/reazen/relude-reason-react/master.svg)](https://circleci.com/gh/reazen/relude-reason-react)
[![npm](https://img.shields.io/npm/v/relude-reason-react.svg)](https://www.npmjs.com/package/relude-reason-react)

# Overview

Relude-based utilities for ReasonReact

# Documentation

## `ReludeReact.Reducer.useReducer` hook

The `ReludeReact.Reducer.useReducer` hook was inspired by the original/pre-hooks [ReasonReact record API](https://reasonml.github.io/reason-react/docs/fr/jsx-2), and the hooks-based [reason-react-update](https://github.com/bloodyowl/reason-react-update) libray by [Matthias Le Brun (bloodyowl)](https://github.com/bloodyowl).

`ReludeReact.Reducer.useReducer` is similar to the [React `useReducer` hook](https://reactjs.org/docs/hooks-reference.html#usereducer) with the key difference that the React `useReducer` only allows
you to change the state, whereas the `ReludeReact.Reducer.useReducer` allows you to both change the state, and to safely emit side effects or `Relude.IO`-based actions, which can result in the emission of further actions.

To use the `ReludeReact.Reducer.useReducer` hook, you must provide a reducer function of the following type:

```reason
type reducer('action, 'state) = ('state, 'action) => update('action, 'state);
```

A function that accepts the current `'state` and an `'action`, and returns a value of type `update('action, 'state)`

The `update` value is a variant with the following type:

```reason
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
```

Basically, this means that in your `ReludeReact.useReducer` component, for any action that occurs,
you can respond to the action by doing any of the following things:

### `NoUpdate`

Don't change the state, and don't perform an side effects or IO-based effects. Basically a no-op - useful as a placeholder or to stub out actions prior to providing the implementations.

### `Update(state)`

Update the component state to the given value, but don't perform any side effects or IO-based effects.

### `UpdateWithSideEffect(state, {state, send} => unit)`

Update the component state to the given value, and perform the given side effect (basically a function that is given a context record of `state` and `send` and is allowed to perform any type of sync or async side effect, and emit additional actions via `send`, which is a function of type `action => unit`, and ultimately return unit `()`. In this case, the side effect is Uncancelable, which means, there is no way to cancel it later.

These types of side effects are useful for doing things like pushing a history state to navigate to a different URL, doing one-off DOM manipulations, or other types of things you don't want or need to manage or control.

### `SideEffect({state, send} => unit)`

Same as `UpdateWithSideEffect`, but with no state update.

### `UpdateWithCancelableSideEffect(state, {state, send} => (unit => unit))`

Same as `UpdateWithSideEffect`, but the side effect can be cancelled via a returned canceler function.

### `CancelableSideEffect({state, send} => (unit, unit))`

Same as `UpdateWithCancelableSideEffect`, but with no state update.

### `UpdateWithIO(state, Relude.IO.t(action, action))`

Similar to `UpdateWithSideEffect`, but instead of a function that accepts the side effect context and returns `()`, you return a `Relude.IO.t('action, 'action)`. An `IO` is a data type which can perform any type of synchronous or asynchronous side effect - see below. `Relude.IO` is a bi-functor which has a typed error channel, and a typed "success" channel. In this case the success and error channels are both constrained to the type `'action`, which means that your `IO`, when executed, must produce an `'action` to dispatch on either success or failure.

A common pattern with component actions is to perform some async action (which typically can fail, e.g. an AJAX/fetch call), and then send a new `'action` when the async invocation either succeeds or fails. This patterns is exactly what's captured by the `Relude.IO.t('action, 'action)` type. See below for a more illustrative example.

### `IO(Relude.IO.t(action, action))`

Similar to `UpdateWithIO`, but with no initial state update.

#### `Relude.IO` Aside

`Relude.IO` is a data type that can be used to execute side effects in a purely functional way.
For those coming from the JavaScript world, you can think of `IO` as something similar to a lazy
promise, but with lots of extra capabilities, brought to you by the power of math and functional programming.

Using `IO` rather than ad-hoc side effect functions gives you all sorts of useful functions for
mapping/flatMapping results and errors, catching and transforming errors, combining multiple async results, and so much more.

See [Relude IO documentation](https://reazen.github.io/relude/#/api/IO) for more information.

## `ReludeReact.Effect` hooks

### `useOnMount`

`ReludeReact.useOnMount` is a simple shortcut which allows you to register a simple `unit => unit` function to run when a component is first mounted. This is typically used to send an initial `'action` into your reducer for initializing the component (e.g. fetch any initial data).

### `useIOOnMount` hook

`ReludeReact.Effect.useIOOnMount` (and it's variations) allows you to trigger a `Relude.IO`-based action when the component is mounted, and handle the final resulting value (either success or failure) using a side-effect callback.

This could be useful if you need to dispatch a fetch request on mount, and then dispatch some reducer actions on success and/or failure, or if you need to store the result of the fetch request
in localStorage, etc.

Variations of this function exist which allow different types of result callbacks - i.e. a callback from `Belt.Result.t('a, 'e) => unit`, separate `'a => unit` and `'e => unit` callbacks, etc.

### `useEffect1WithEq`...`useEffect5WithEq`

These effect hooks are very similar to their `React.useEffectN` counterparts, except that you provide your own equality function along with any values the hook depends upon for re-running.

React's `useEffect` dependencies are simply checked by `(===)`, which is fast but may lead to false positives when deciding if a hook dependencies have changed (particularly with complex types like records and lists). In cases where running an effect may be expensive, `useEffectNWithEq` allows much more control over whether that effect should run.

## `ReludeReact.Render` utilities

`ReludeReact.Render` contains a variety of useful functions for rendering different data types, to avoid extra boilerplate/noise in your components. The purpose of these functions is to try to streamline conditional rendering, so you don't have to write lots of `_ => React.null` cases when rendering conditional values, variants like `Relude.AsyncResult.t('a, 'e)`, etc.

```reason
ReludeReact.Render.ifTrue
ReludeReact.Render.ifTrueLazy
ReludeReact.Render.ifFalse
ReludeReact.Render.ifFalseLazy
ReludeReact.Render.option
ReludeReact.Render.optionLazy
ReludeReact.Render.optionIfSome
ReludeReact.Render.result
ReludeReact.Render.resultIfOk
ReludeReact.Render.resultIfError
ReludeReact.Render.asyncData
ReludeReact.Render.asyncDataLazy
ReludeReact.Render.asyncDataByValue
ReludeReact.Render.asyncDataLazyByValue
ReludeReact.Render.asyncResult
ReludeReact.Render.asyncResultLazy
ReludeReact.Render.asyncResultByValue
ReludeReact.Render.asyncResultLazyByValue
// And many more!
```

## Examples

See the demo app in `examples/demo`

```
> npm run demo
```

Below is a somewhat contrived/simple example of what a `ReludeReact` component might look like.

```reason
// AnimalListView.re
module AsyncResult = Relude.AsyncResult;
module IO = Relude.IO;
module List = Relude.List;

// The state of this component
// We're using a Relude.AsyncResult to represent the state of animals, which are loaded asynchronously and can fail.
type state = {
  title: string,
  animalsResult: AsyncResult.t(list(Animal.t), Error.t),
};

// The initial state for the component (used in the reducer initialization below)
let initialState = {title: "Animals", animalsResult: AsyncResult.init};

// The actions that our component emits and handles in the reducer
type action =
  | FetchAnimals
  | FetchAnimalsSuccess(list(Animal.t))
  | FetchAnimalsError(Error.t)
  | ViewCreateForm
  | ViewAnimal(Animal.t)
  | DeleteAnimal(Animal.t)
  | NoOp;

// The reducer function which accepts and action and the current state, and emits
// an "update" which can do things like updating the state, running raw or IO-based effects
let reducer =
    (action: action, state: state): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchAnimals =>
    UpdateWithIO(
      {...state, animalsResult: state.animalsResult |> AsyncResult.toBusy},
      API.fetchAnimals
      |> IO.bimap(a => FetchAnimalsSuccess(a), e => FetchAnimalsError(e)),
    )

  | FetchAnimalsSuccess(animals) =>
    Update({...state, animalsResult: AsyncResult.completeOk(animals)})

  | FetchAnimalsError(error) =>
    Update({...state, animalsResult: AsyncResult.completeError(error)})

  | ViewCreateForm => SideEffect(_ => ReasonReactRouter.push("/create"))

  | ViewAnimal(_animal) => NoUpdate

  | DeleteAnimal(_animal) => NoUpdate

  | NoOp => NoUpdate
  };

// Various inline components

module AnimalsLoading = {
  [@react.component]
  let make = () => {
    <div> {React.string("Loading animals...")} </div>;
  };
};

module AnimalsTable = {
  [@react.component]
  let make = (~animals: list(Animal.t), ~send: action => unit) => {
    let _ = send; // TODO
    <div>
      {React.string("Animals: " ++ string_of_int(List.length(animals)))}
    </div>;
  };
};

module AnimalsError = {
  [@react.component]
  let make = (~error: Error.t) =>
    <div> {React.string(Error.show(error))} </div>;
};

module AnimalsResult = {
  [@react.component]
  let make = (~result: AsyncResult.t(list(Animal.t), Error.t), ~send) =>
    result
    |> ReludeReact.Render.asyncResultByValueLazy(
         _ => <AnimalsLoading />,
         animals => <AnimalsTable animals send />,
         error => <AnimalsError error />,
       );
};

// The main view - accepts the state and send values we get from the reducer

module Main = {
  [@react.component]
  let make = (~state, ~send) => {
    <div>
      <h1> {React.string(state.title)} </h1>
      <div>
        <button
          onClick={e => {
            ReactEvent.Synthetic.preventDefault(e);
            send(ViewCreateForm);
          }}>
          {React.string("Create")}
        </button>
        <button
          href="#"
          onClick={e => {
            ReactEvent.Synthetic.preventDefault(e);
            send(NoOp);
          }}>
          {React.string("No-Op Action")}
        </button>
      </div>
      <AnimalsResult send result={state.animalsResult} />
    </div>;
  };
};

// The main component definition
// Here, we invoke our hooks and render the main view

[@react.component]
let make = () => {
  // Initialize the ReludeReact reducer
  let (state, send) = ReludeReact.Reducer.useReducer(initialState, reducer);

  // Trigger an initialization action on mount
  // This is just using the send function from our reducer to send an action, which is handled by the reducer
  ReludeReact.Effect.useOnMount(() => send(FetchAnimals));

  // This is just demonstrating triggering an IO action on mount, and handling the result via side-effecting functions
  // In reality, the IO would probably be making a fetch request, or doing some other async action and then storing or
  // dispatching the results.
  ReludeReact.Effect.useIOOnMount(
    IO.suspend(() => {
      Js.log("Suspend 42");
      42;
    }),
    intValue => Js.log("Got suspended value: " ++ string_of_int(intValue)),
    _error => Js.log("Suspend 42 failed"),
  );

  // Render our main view, passing the state and dispatcher function down
  <Main state send />;
};
```

## Developer info

### Project setup

```
> git clone git@github.com:reazen/relude-reason-react
> cd relude-reason-react
> npm install
> npm run server:demo
```

### Scripts

```
> npm run clean
> npm run build
> npm run cleanbuild
> npm run test
> npm run cleantest
> npm run watch
> npm run demo
```

### Publishing to npm

```
> npm version major|minor|patch
> git push origin --follow-tags
> git push upstream --follow-tags
> npm publish
```

### NixOS

If you have trouble building/installing the Bucklescript/Reason tools try this:

```
> nix-shell
%nix%> npm install
```
