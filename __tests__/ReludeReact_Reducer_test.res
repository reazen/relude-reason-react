open Jest
open Expect

module TestingLibrary = {
  type renderResult
  type element 

  type match = [#Str(string)]

  @module("@testing-library/react") external render: (React.element) => renderResult = "render"
  @send external _getByText: (renderResult, string) => element = "getByText"
  @module("@testing-library/dom") @scope("fireEvent") external click: (element) => unit = "click"

  let getByText = (renderResult, #Str(text)) => _getByText(renderResult, text)
}

module Counter = {
  type state = int

  let clickCount = ref(0) // tracks updates as plain old side effects

  let initialState = 0

  type action = Inc

  let reducer: ReludeReact.Reducer.reducer<action, state> = (state: state, action: action) => {
    switch action {
    | Inc =>
      // Note: This is not how effects are meant to be done, this is just testing some
      // assumptions about React's default behavior
      clickCount := clickCount.contents + 1
      Update(state + 1)
    }
  }

  @react.component
  let make = () => {
    let (state, send) = ReludeReact_Reducer.useReducer(reducer, initialState)

    <div>
      {React.string("Count: " ++ string_of_int(state))}
      <button type_="button" onClick={_ => send(Inc)}> {React.string("Increment")} </button>
    </div>
  }
}

describe("ReludeReact_Reducer", () => {
  // This tests the current "expected" behavior of useReducer in terms of how it handles
  // unmanaged side effects. The expected behavior is that uncontrolled side effects might
  // run multiple times (in this case, two times). This behavior is not desirable, but according to
  // a tweet by Dan Abramov, it should be expected. Overall, it's discouraged to have
  // unmanaged side effects in the reducer, so this is not something we would normally run into
  // if we're using the Relude SideEffect and IO-based update commands.
  test(
    "Reducer side effect might occur multiple times (so don't do side effects like this)",
    () => {
      let renderResult = TestingLibrary.render(<Counter />)
      renderResult
      |> TestingLibrary.getByText(_, #Str("Increment"))
      |> TestingLibrary.click(_)
      expect(Counter.clickCount.contents) |> toEqual(2)
    },
  )
})
