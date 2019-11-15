//open Relude.Globals;

type state = {count: int};

let initialState = {count: 0};

type action =
  | Inc
  | Dec;

let reducer: ReludeReact.Reducer.reducer(action, state) =
  (state, action) =>
    switch (action) {
    | Inc =>
      // This seems to work as expected - a single log
      UpdateWithSideEffect({count: state.count + 1}, _ => Js.log("Inc"));

    | Dec =>
      Js.log("Dec"); // This logs "Dec" twice if you click the Decrement button first before doing anything else
      Update({count: state.count - 1});
    };

[@react.component]
let make = () => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  <div>
    <div> {React.string("Count: " ++ string_of_int(state.count))} </div>
    <button
      type_="button"
      onClick={_ => {
        Js.log("onClick Inc");
        send(Inc);
      }}>
      {React.string("Increment")}
    </button>
    <button
      type_="button"
      onClick={_ => {
        Js.log("onClick Dec");
        send(Dec);
      }}>
      {React.string("Decrement")}
    </button>
  </div>;
};