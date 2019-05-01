[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  switch (url.path) {
  | [] => <AnimalListView />

  //| ["animals", name] => <AnimalView name />

  | _ => <div> {React.string("Invalid URL")} </div>
  };
};