open Jest;
open Expect;
open ReactTestingLibrary;

module Counter = {
  type state = int;

  let initialState = 0;

  type action =
    | Inc;

  let reducer: ReludeReact.Reducer.reducer(action, state) =
    (state: state, action: action) => {
      switch (action) {
      | Inc =>
        Js.log("Inc");
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

describe("ReludeReact", () => {
  test("one", () =>
    pass
  );
  test("two", () =>
    expect(true) |> toBe(true)
  );

  test("three", () => {
    expect(ReludeReact_Render.ifTrue(<br />, false)) |> toEqual(React.null)
  });

  test("Reducer should be called once", () => {
    let wrapper = render(<Counter />);

    let _ = [%raw {|jest.spyOn(console, 'log').mockImplementation(() => {})|}];
    let _ = [%raw {|expect(console.log).toHaveBeenCalledTimes(0)|}];

    wrapper |> getByText(~matcher=`Str("Increment")) |> FireEvent.click;

    let _ = [%raw {|expect(console.log).toHaveBeenCalledTimes(1)|}];
    pass;
  });
});