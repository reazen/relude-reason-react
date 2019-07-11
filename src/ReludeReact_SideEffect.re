/**
Side effects are given a context argument of the current state, and a dispatch function
for sending 0 or more additional actions for later processing.
*/
type context('action, 'state) = {
  state: 'state,
  send: 'action => unit,
};

/**
A canceler function is a basic function with no arguments and no result, which can
be called to cancel a running or future side effect.
*/
type canceler = unit => unit;

/**
A SideEffect.t is a function that takes a context (the current state, and an action dispatcher function),
performs a side effect with result type type unit and returns an optional canceler function.
The side effect should typically send one or more actions via the dispatcher `send` function,
in order to further affect the system.
*/
type t('action, 'state) = context('action, 'state) => option(canceler);

/**
Alias for SideEffect.t
*/
type sideEffect('action, 'state) = t('action, 'state);

module Uncancelable = {
  /**
  An Uncancelable.t side effect is a side effect which cannot be canceled.
   */
  type t('action, 'state) = context('action, 'state) => unit;

  /**
  Lifts an uncancelable side effect to the more general SideEffect.t type
  */
  let lift: t('action, 'state) => sideEffect('action, 'state) =
    (uncancelable, context) => {
      uncancelable(context);
      None;
    };

  module IO = {
    /**
    An uncancellable side effect represented as a `Relude.IO` which succeeds with an `action` to
    perform, and fails with a possibly different `action` to perform.
    */
    type t('action, 'state) = Relude.IO.t('action, 'action);
  };
};

module Cancelable = {
  /**
  A Cancelable.t is a side effect which can be canceled using the returned canceler function.
  */
  type t('action, 'state) = context('action, 'state) => canceler;

  /**
  Lifts an cancelable side effect to the more general sideEffect type
  */
  let lift: t('action, 'state) => sideEffect('action, 'state) =
    (cancelable, context) => Some(cancelable(context));
  // Note: we don't have cancelable IO yet
};