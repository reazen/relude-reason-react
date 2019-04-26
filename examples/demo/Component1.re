let handleClick = (_event) => Js.log("clicked!");

[@react.component]
let make = (~message) =>
  <>
    <h1>{ReasonReact.string(ReludeReact.title)}</h1>
    <div onClick={handleClick}>
      {ReasonReact.string(message)}
    </div>
  </>;
