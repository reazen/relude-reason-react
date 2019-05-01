type context('action, 'state) = {
  state: 'state,
  send: 'action => unit,
};

type canceller = unit => unit;

type t('action, 'state) = {
  unsafeRun: context('action, 'state) => option(canceller),
};

type sideEffect('action, 'state) = t('action, 'state);

module Uncancellable = {
  type t('action, 'state) = {run: context('action, 'state) => unit};

  let lift: t('action, 'state) => sideEffect('action, 'state) =
    uncancellable => {
      unsafeRun: context => {
        uncancellable.run(context);
        None;
      },
    };

  type io('action, 'state) = Relude.IO.t('action, 'action);
};

module Cancellable = {
  type t('action, 'state) = {run: context('action, 'state) => canceller};

  let lift: t('action, 'state) => sideEffect('action, 'state) =
    cancellable => {unsafeRun: context => Some(cancellable.run(context))};
};