type state = {
  title: string,
  animalsResult: Relude.AsyncResult.t(list(Animal.t), Error.t),
};

type action =
  | FetchAnimals
  | FetchAnimalsSuccess(list(Animal.t))
  | FetchAnimalsError(Error.t)
  | ViewCreateForm
  | ViewAnimal(Animal.t)
  | DeleteAnimal(Animal.t);

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

[@react.component]
let make = () => {
  let (state, send) =
    ReludeReact.Reducer.useReducer(
      {title: "Animals", animalsResult: Relude.AsyncResult.init},
      (action, state) =>
      switch (action) {
      | FetchAnimals =>
        UpdateWithIO(
          {
            ...state,
            animalsResult: state.animalsResult |> Relude.AsyncResult.toBusy,
          },
          API.fetchAnimals
          |> Relude.IO.bimap(
               a => FetchAnimalsSuccess(a),
               e => FetchAnimalsError(e),
             ),
        )

      | FetchAnimalsSuccess(animals) =>
        Update({
          ...state,
          animalsResult: Relude.AsyncResult.completeOk(animals),
        })

      | FetchAnimalsError(error) =>
        Update({
          ...state,
          animalsResult: Relude.AsyncResult.completeError(error),
        })

      | ViewCreateForm => SideEffect(_ => ReasonReactRouter.push("/create"))

      | ViewAnimal(_animal) => NoUpdate

      | DeleteAnimal(_animal) => NoUpdate
      }
    );

  ReludeReact.Effect.useOnMount(() => send(FetchAnimals));

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