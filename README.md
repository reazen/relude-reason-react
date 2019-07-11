# relude-reason-react

[![CircleCI](https://img.shields.io/circleci/project/github/reazen/relude-reason-react/master.svg)](https://circleci.com/gh/reazen/relude-reason-react)
[![npm](https://img.shields.io/npm/v/relude-reason-react.svg)](https://www.npmjs.com/package/relude-reason-react)

## Overview

Relude-based utilities for ReasonReact

## Documentation

### `ReludeReact.useReducer`

The `ReludeReact.Reducer` was inspired by the original/pre-hooks [ReasonReact record API](https://reasonml.github.io/reason-react/docs/fr/jsx-2), and the hooks-based [reason-react-update](https://github.com/bloodyowl/reason-react-update) libray by [Matthias Le Brun (bloodyowl)](https://github.com/bloodyowl).

`ReludeReact.useReducer` is similar to the [React `useReducer` hook](https://reactjs.org/docs/hooks-reference.html#usereducer) with the key difference that the React `useReducer` only allows
you to change the state, whereas the `ReludeReact.useReducer` allows you to both change the state, and to safely emit side effects or `Relude.IO`-based actions, which can result in the emission of further actions.

To use the `ReludeReact.useReducer` hook, you must provide a reducer function of the following type:

```reason
type reducer('action, 'state) = ('action, 'state) => update('action, 'state);
```

A function that accepts an `'action` and the current `'state`, and returns a value of type `update('action, 'state)`

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

```reason
NoUpdate
```

Don't change the state, and don't perform an side effects or IO-based effects

```reason
Update(state)
```

Update the component state to the given value, but don't perform any side effects or IO-based effects.

```reason
UpdateWithSideEffect(state, sideEffect)
```

Update the component state to the given value, and perform the given side effect (basically a function that is given `state` and `send` and is allowed to perform any type of sync or async side effect, and emit additional actions via `send`, and ultimately return unit `()`.  In this case, the side effect is Uncancelable, which means, there is no way to cancel it later.

These types of side effects are useful for doing things like pushing a history state to navigate to a different URL, doing one-off DOM manipulations, or other types of things you don't want or need to manage or control.

```reason
SideEffect(sideEffect)
```

Same as `UpdateWithSideEffect`, but with no state update.

```reason
UpdateWithCancelableSideEffect(state, sideEffect)
```

Same as `UpdateWithSideEffect`, but the side effect can be cancelled via a returned canceler function.

```reason
CancelableSideEffect(sideEffect)
```

Same as `UpdateWithCancelableSideEffect`, but with no state update.

```reason
UpdateWithIO(state, ioOfActionAction)
```

Similar to `UpdateWithSideEffect`, but instead of a function that accepts the side effect context and returns `()`, you return a `Relude.IO.t('action, 'action)`.  An `IO` is a data type which can perform any type of synchronous or asynchronous side effect - see below.  `Relude.IO` is a bi-functior which has a typed error channel, and a typed "success" channel.  In this case the success and error channels are both constrained to the type `'action`, which means that your `IO`, when executed, must produce an `'action` to dispatch on either success or failure.

A common pattern with component actions is to perform some async action (which typically can fail, e.g. an AJAX/fetch call), and then send a new `'action` when the async invocation either succeeds or fails.  This patterns is exactly what's captured by the `Relude.IO.t('action, 'action)` type.  See below for a more illustrative example.

```reason
IO(ioOfActionAction)
```

Similar to `UpdateWithIO`, but with no initial state update.

### `Relude.IO` Aside

`Relude.IO` is a data type that can be used to execute side effects in a purely functional way.
For those coming from the JavaScript world, you can think of `IO` as something similar to a lazy
promise, but with lots of extra capabilities, brought to you by the power of math and functional programming.

Using `IO` rather than ad-hoc side effect functions gives you all sorts of useful functions for
mapping/flatMapping results and errors, catching and transforming errors, combining multiple async results, and so much more.

See [Relude IO documentation](https://reazen.github.io/relude/#/api/IO) for more information.

### `ReludeReact.useOnMount`

`ReludeReact.useOnMount` is a simple shortcut which allows you to register a simple `unit => unit` function to run when a component is first mounted.  This is typically used to send an initial `'action` into your reducer for initializing the component (e.g. fetch any initial data).

### `ReludeReact.Render` utilities

TODO

## Examples

See the demo app in `examples/demo`

```
> npm run demo
```

Below is a somewhat contrived/simple example fo what a `ReludeReact` component might look like.

```reason
// AnimalListView.re

// The state of your component.
// Relude.AsyncResult is a type similar to Elm's RemoteData - a variant which
// allows you to represent data in various states of loading (init/loading/loaded/reloading/failed).
type state = {
  title: string,
  animalsResult: Relude.AsyncResult.t(list(Animal.t), Error.t),
};

// The actions which the component emits and handles via the ReludeReact.useReducer hook
type action =
  | FetchAnimals
  | FetchAnimalsSuccess(list(Animal.t))
  | FetchAnimalsError(Error.t)
  | ViewCreateForm
  | ViewAnimal(Animal.t)
  | DeleteAnimal(Animal.t);

// Render functions for various parts of the component
// Note: these could also be nested components here.

let renderAnimalsLoading = () => {
  <div> {React.string("Loading animals...")} </div>;
};

let renderAnimalsTable = (_send: action => unit, animals: list(Animal.t)) =>
  <div>
    {React.string("Animals: " ++ string_of_int(Relude.List.length(animals)))}
  </div>;

let renderAnimalsError = (error: Error.t) =>
  <div> {React.string(Error.show(error))} </div>;

let renderAnimalsResult = (send, result) =>
  result
  |> Relude.AsyncResult.foldByValueLazy(
       renderAnimalsLoading,
       renderAnimalsTable(send),
       renderAnimalsError,
     );

// This is our main component
[@react.component]
let make = () => {
  let (state, send) =

    // Initialize the ReludeReact.Reducer hook
    // We provide the initial state, and a function `(action, send) => update`
    // Note: this reducer function can be defined above/outside this function -
    // it just needs to be passed the `state` and `send` arguments (if needed).
    ReludeReact.Reducer.useReducer(
      // initial state (can also be defined outside this function)
      {title: "Animals", animalsResult: Relude.AsyncResult.init},

      // the reducer function - takes an action and the current state, and returns
      // an `update` value, which is then processed for side effects.
      (action, state) =>
      switch (action) {
      | FetchAnimals =>
        UpdateWithIO(
          // New state to apply now
          {
            ...state,
            animalsResult: state.animalsResult |> Relude.AsyncResult.toBusy,
          },
          // Create an IO.t('action, 'action) to run - the resulting actions (error or success) will be sent automatically
          // Here we are making an async API call (which returns an IO), and then emitting a Success
          // or Error action with the API result.
          API.fetchAnimals
          |> Relude.IO.bimap(
               a => FetchAnimalsSuccess(a),
               e => FetchAnimalsError(e),
             ),
        )

      | FetchAnimalsSuccess(animals) =>
        // Basic state update
        Update({
          ...state,
          animalsResult: Relude.AsyncResult.completeOk(animals),
        })

      | FetchAnimalsError(error) =>
        // Basic state update
        Update({
          ...state,
          animalsResult: Relude.AsyncResult.completeError(error),
        })

      // This is a navigation action, so we simply perform an unmanaged side effect
      // which takes us to a new page (component) via a router managing this component.
      | ViewCreateForm => SideEffect(_ => ReasonReactRouter.push("/create"))

      // TODO: NoUpdate for now
      | ViewAnimal(_animal) => NoUpdate

      // TODO: NoUpdate for now
      | DeleteAnimal(_animal) => NoUpdate
      }
    );

  // Use the onMount convenience hook as a way to initialize the component via an async action.
  ReludeReact.Effect.useOnMount(() => send(FetchAnimals));

  // Here is our main view
  <div>
    <h1> {React.string(state.title)} </h1>
    <div>
      <a
        href="#"
        onClick={e => {
          ReactEvent.Synthetic.preventDefault(e);
          send(ViewCreateForm);
        }}>
        {React.string("Create")}
      </a>
    </div>
    {renderAnimalsResult(send, state.animalsResult)}
  </div>;
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
