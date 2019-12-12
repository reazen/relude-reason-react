open Jest;
open Expect;

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
});