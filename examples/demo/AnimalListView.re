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