open Jest;
open Expect;
open ReactTestingLibrary;

// This component tests the behavior of useReducer(~useRef=false, ...)
// false is the default, so the useRef parameter is omitted.
module Counter = {
  type state = int;

  let clickCount = ref(0); // tracks updates as plain old side effects

  let initialState = 0;

  type action =
    | Inc;

  let reducer: ReludeReact.Reducer.reducer(action, state) =
    (state: state, action: action) => {
      switch (action) {
      | Inc =>
        // Note: This is not how effects are meant to be done, this is just testing some
        // react behavior
        clickCount := clickCount^ + 1;
        Update(state + 1);
      };
    };

  [@react.component]
  let make = () => {
    let (state, send) =
      ReludeReact_Reducer.useReducer(reducer, initialState);

    <div>
      {React.string("Count: " ++ string_of_int(state))}
      <button type_="button" onClick={_ => send(Inc)}>
        {React.string("Increment")}
      </button>
    </div>;
  };
};

// This component tests the behavior of useReducer(~useRef=true, ...)
module CounterWithRef = {
  type state = int;

  let clickCount = ref(0); // tracks updates as plain old side effects

  let initialState = 0;

  type action =
    | Inc;

  let reducer: ReludeReact.Reducer.reducer(action, state) =
    (state: state, action: action) => {
      switch (action) {
      | Inc =>
        // Note: This is not how effects are meant to be done, this is just testing some
        // react behavior
        clickCount := clickCount^ + 1;
        Update(state + 1);
      };
    };

  [@react.component]
  let make = () => {
    let (state, send) =
      ReludeReact_Reducer.useReducer(~useRef=true, reducer, initialState);

    <div>
      {React.string("Count: " ++ string_of_int(state))}
      <button type_="button" onClick={_ => send(Inc)}>
        {React.string("Increment")}
      </button>
    </div>;
  };
};

describe("ReludeReact_Reducer", () => {
  // This tests the current "expected" behavior of useReducer in terms of how it handles
  // uncontrolled side effects. The expected behavior is that uncontrolled side effects might
  // run multiple times (in this case 2). This behavior is maybe not desired, but according to
  // a casual tweet by Dan Abramov, it should be expected. Overall, it's discouraged to have
  // unmanaged side effects in the reducer, so this is not something we would normally run into
  // if we're using the Relude SideEffect and IO-based update commands.
  test("Reducer should be called twice when not using ref", () => {
    let renderResult = render(<Counter />);
    renderResult |> getByText(~matcher=`Str("Increment")) |> FireEvent.click;
    expect(Counter.clickCount^) |> toEqual(2);
  });

  // This is testing our experimental "useRef" option for the
  // ReludeReact.Reducer.useReducer. This prevents unmanaged side effects from
  // running more than once, but might cause some other unrelated problems, like
  // with unwanted closure/memoization of input props. These other problems have not
  // yet be fully diagnosed.
  test("Reducer should be called once when using ref wrapper", () => {
    let renderResult = render(<CounterWithRef />);
    renderResult |> getByText(~matcher=`Str("Increment")) |> FireEvent.click;
    expect(CounterWithRef.clickCount^) |> toEqual(1);
  });
});