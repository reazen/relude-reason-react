open Jest
open Expect

describe("ReludeReact_Render", () => {
  test("Render.ifTrue true", () =>
    expect(ReludeReact_Render.ifTrue(<br />, true)) |> toEqual(<br />)
  )

  test("Render.ifTrue false", () =>
    expect(ReludeReact_Render.ifTrue(<br />, false)) |> toEqual(React.null)
  )
})
