type context('action, 'state) = {
  state: 'state,
  send: 'action => unit,
};

type canceller = unit => unit;

type t('action, 'state) = {
  unsafeRun: context('action, 'state) => option(canceller),
};

// Alias for SideEffect.t
type sideEffect('action, 'state) = t('action, 'state);

module Uncancellable = {
  type t('action, 'state) = {unsafeRun: context('action, 'state) => unit};

  // Lifts an uncancellable side effect to the more general sideEffect type
  let lift: t('action, 'state) => sideEffect('action, 'state) =
    uncancellable => {
      unsafeRun: context => {
        uncancellable.unsafeRun(context);
        None;
      },
    };

  type io('action, 'state) = Relude.IO.t('action, 'action);
};

module Cancellable = {
  type t('action, 'state) = {
    unsafeRun: context('action, 'state) => canceller,
  };

  // Lifts an cancellable side effect to the more general sideEffect type
  let lift: t('action, 'state) => sideEffect('action, 'state) =
    cancellable => {
      unsafeRun: context => Some(cancellable.unsafeRun(context)),
    };
};