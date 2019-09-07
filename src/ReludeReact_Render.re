/**
Returns an element if the condition is true, otherwise React.null
*/
let ifTrue: (React.element, bool) => React.element =
  (element, condition) => if (condition) {element} else {React.null};

/**
Renders an element if the condition is true, otherwise React.null
*/
let ifTrueLazy: (unit => React.element, bool) => React.element =
  (element, condition) =>
    if (condition) {
      element();
    } else {
      React.null;
    };

/**
Returns an element if the condition is false, otherwise React.null
*/
let ifFalse: (React.element, bool) => React.element =
  (element, condition) =>
    if (!condition) {
      element;
    } else {
      React.null;
    };

/**
Renders an element if the condition is false, otherwise React.null
*/
let ifFalseLazy: (unit => React.element, bool) => React.element =
  (element, condition) =>
    if (!condition) {
      element();
    } else {
      React.null;
    };

/**
Renders an option with a None element or render fucntion for Some
 */
let option:
  'a.
  (React.element, 'a => React.element, option('a)) => React.element
 =
  (noneElement, someElement, opt) =>
    switch (opt) {
    | None => noneElement
    | Some(a) => someElement(a)
    };

/**
Renders an option with render functions for None and Some
 */
let optionLazy:
  'a.
  (unit => React.element, 'a => React.element, option('a)) => React.element
 =
  (noneElement, someElement, opt) =>
    switch (opt) {
    | None => noneElement()
    | Some(a) => someElement(a)
    };

/**
Renders an option with a render function for Some, otherwise React.null
 */
let optionIfSome: 'a. ('a => React.element, option('a)) => React.element =
  (someElement, opt) =>
    switch (opt) {
    | None => React.null
    | Some(a) => someElement(a)
    };

/**
Renders an option with a render function for None, otherwise React.null
 */
let optionIfNone: 'a. (React.element, option('a)) => React.element =
  (element, opt) =>
    switch (opt) {
    | None => element
    | Some(_) => React.null
    };

/**
Renders an option with a render function for None, otherwise React.null
 */
let optionIfNoneLazy: 'a. (unit => React.element, option('a)) => React.element =
  (element, opt) =>
    switch (opt) {
    | None => element()
    | Some(_) => React.null
    };

/**
Renders a Belt.Result.t('a, 'e), with render functions for both channels
 */
let result:
  'a 'e.
  ('a => React.element, 'e => React.element, Belt.Result.t('a, 'e)) =>
  React.element
 =
  (okElement, errorElement, result) =>
    switch (result) {
    | Ok(a) => okElement(a)
    | Error(e) => errorElement(e)
    };

/**
Renders a Belt.Result.t('a, 'e), with a render function for the Ok channel, otherwise React.null
 */
let resultIfOk:
  'a 'e.
  ('a => React.element, Belt.Result.t('a, 'e)) => React.element
 =
  (okElement, result) =>
    switch (result) {
    | Ok(a) => okElement(a)
    | Error(_) => React.null
    };

/**
Renders a Belt.Result.t('a, 'e), with a render function for the Error channel, otherwise React.null
 */
let resultIfError:
  'a 'e.
  ('e => React.element, Belt.Result.t('a, 'e)) => React.element
 =
  (errorElement, result) =>
    switch (result) {
    | Ok(_) => React.null
    | Error(e) => errorElement(e)
    };

/**
Renders a Relude.AsyncData value by providing an element/render function for each case
*/
let asyncData:
  'a.
  (
    React.element,
    React.element,
    'a => React.element,
    'a => React.element,
    Relude.AsyncData.t('a)
  ) =>
  React.element
 =
  (initElement, loadingElement, reloadingElement, completeElement, data) =>
    switch (data) {
    | Init => initElement
    | Loading => loadingElement
    | Reloading(a) => reloadingElement(a)
    | Complete(a) => completeElement(a)
    };

/**
Renders a Relude.AsyncData value by providing a render function for each case
*/
let asyncDataLazy:
  'a.
  (
    unit => React.element,
    unit => React.element,
    'a => React.element,
    'a => React.element,
    Relude.AsyncData.t('a)
  ) =>
  React.element
 =
  (initElement, loadingElement, reloadingElement, completeElement, data) =>
    switch (data) {
    | Init => initElement()
    | Loading => loadingElement()
    | Reloading(a) => reloadingElement(a)
    | Complete(a) => completeElement(a)
    };

/**
Renders a Relude.AsyncData value by providing an element/render function for each type of same-type value
*/
let asyncDataByValue:
  'a.
  (React.element, 'a => React.element, Relude.AsyncData.t('a)) =>
  React.element
 =
  (initOrLoadingElement, valueElement, data) =>
    switch (data) {
    | Init
    | Loading => initOrLoadingElement
    | Reloading(a)
    | Complete(a) => valueElement(a)
    };

/**
Renders a Relude.AsyncData value by providing a render function for each type of same-type value
*/
let asyncDataByValueLazy:
  'a.
  (unit => React.element, 'a => React.element, Relude.AsyncData.t('a)) =>
  React.element
 =
  (initOrLoadingElement, valueElement, data) =>
    switch (data) {
    | Init
    | Loading => initOrLoadingElement()
    | Reloading(a)
    | Complete(a) => valueElement(a)
    };

/**
Renders a Relude.AsyncResult value by providing an element/render function for each case
*/
let asyncResult:
  'a 'e.
  (
    React.element,
    React.element,
    'a => React.element,
    'e => React.element,
    'a => React.element,
    'e => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (
    initElement,
    loadingElement,
    reloadingOkElement,
    reloadingErrorElement,
    completeOkElement,
    completeErrorElement,
    result,
  ) =>
    switch (result) {
    | Init => initElement
    | Loading => loadingElement
    | Reloading(Ok(a)) => reloadingOkElement(a)
    | Reloading(Error(e)) => reloadingErrorElement(e)
    | Complete(Ok(a)) => completeOkElement(a)
    | Complete(Error(e)) => completeErrorElement(e)
    };

/**
Renders a Relude.AsyncResult value by providing a render function for each case
*/
let asyncResultLazy:
  'a 'e.
  (
    unit => React.element,
    unit => React.element,
    'a => React.element,
    'e => React.element,
    'a => React.element,
    'e => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (
    initElement,
    loadingElement,
    reloadingOkElement,
    reloadingErrorElement,
    completeOkElement,
    completeErrorElement,
    result,
  ) =>
    switch (result) {
    | Init => initElement()
    | Loading => loadingElement()
    | Reloading(Ok(a)) => reloadingOkElement(a)
    | Reloading(Error(e)) => reloadingErrorElement(e)
    | Complete(Ok(a)) => completeOkElement(a)
    | Complete(Error(e)) => completeErrorElement(e)
    };

/**
Renders a Relude.AsyncResult value by providing an element/render function for each same-type value
*/
let asyncResultByValue:
  'a 'e.
  (
    React.element,
    'a => React.element,
    'e => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (initOrLoadingElement, okElement, errorElement, result) =>
    switch (result) {
    | Init
    | Loading => initOrLoadingElement
    | Reloading(Ok(a))
    | Complete(Ok(a)) => okElement(a)
    | Reloading(Error(e))
    | Complete(Error(e)) => errorElement(e)
    };

/**
Renders a Relude.AsyncResult value by providing a render function for each same-type value
*/
let asyncResultByValueLazy:
  'a 'e.
  (
    unit => React.element,
    'a => React.element,
    'e => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (initOrLoadingElement, okElement, errorElement, result) =>
    switch (result) {
    | Init
    | Loading => initOrLoadingElement()
    | Reloading(Ok(a))
    | Complete(Ok(a)) => okElement(a)
    | Reloading(Error(e))
    | Complete(Error(e)) => errorElement(e)
    };

/**
Renders a Relude.AsyncResult if it's in the Init state
*/
let asyncResultIfInit:
  'a 'e.
  (React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init => element
    | Loading => React.null
    | Reloading(_) => React.null
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in the Init state
*/
let asyncResultIfInitLazy:
  'a 'e.
  (unit => React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init => element()
    | Loading => React.null
    | Reloading(_) => React.null
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in the Loading state
*/
let asyncResultIfLoading:
  'a 'e.
  (React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init => React.null
    | Loading => element
    | Reloading(_) => React.null
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in the Loading state
*/
let asyncResultIfLoadingLazy:
  'a 'e.
  (unit => React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init => React.null
    | Loading => element()
    | Reloading(_) => React.null
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in the Reloading state
*/
let asyncResultIfReloading:
  'a 'e.
  (Belt.Result.t('a, 'e) => React.element, Relude.AsyncResult.t('a, 'e)) =>
  React.element
 =
  (element, result) =>
    switch (result) {
    | Init => React.null
    | Loading => React.null
    | Reloading(r) => element(r)
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in the Complete state
*/
let asyncResultIfComplete:
  'a 'e.
  (Belt.Result.t('a, 'e) => React.element, Relude.AsyncResult.t('a, 'e)) =>
  React.element
 =
  (element, result) =>
    switch (result) {
    | Init => React.null
    | Loading => React.null
    | Reloading(_) => React.null
    | Complete(r) => element(r)
    };

/**
Renders a Relude.AsyncResult if there is no contained value (i.e. for Init or Loading states)
 */
let asyncResultIfEmpty:
  'a 'e.
  (React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init
    | Loading => element
    | _ => React.null
    };

/**
Renders a Relude.AsyncResult if there is no contained value (i.e. for Init or Loading states)
 */
let asyncResultIfEmptyLazy:
  'a 'e.
  (unit => React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Init
    | Loading => element()
    | _ => React.null
    };

/**
Renders a Relude.AsyncResult if it's in a state that contains a Belt.Result value (i.e. Reloading or Complete)
 */
let asyncResultIfNonEmpty:
  'a 'e.
  (Belt.Result.t('a, 'e) => React.element, Relude.AsyncResult.t('a, 'e)) =>
  React.element
 =
  (element, result) =>
    switch (result) {
    | Complete(r)
    | Reloading(r) => element(r)
    | _ => React.null
    };

/**
Renders a Relude.AsyncResult if there is an `Ok` value (either Reloading or Complete)
 */
let asyncResultIfOk:
  'a 'e.
  ('a => React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Reloading(Ok(a))
    | Complete(Ok(a)) => element(a)
    | _ => React.null
    };

/**
Renders a Relude.AsyncResult if there is an `Error` value (either Reloading or Complete)
 */
let asyncResultIfError:
  'a 'e.
  ('e => React.element, Relude.AsyncResult.t('a, 'e)) => React.element
 =
  (element, result) =>
    switch (result) {
    | Reloading(Error(e))
    | Complete(Error(e)) => element(e)
    | _ => React.null
    };

/**
Renders a Relude.AsyncResult if it's in a working (busy) state (e.g. Loading or Reloading).
 */
let asyncResultIfBusy:
  'a 'e.
  (
    option(Belt.Result.t('a, 'e)) => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (element, result) =>
    switch (result) {
    | Init => React.null
    | Loading => element(None)
    | Reloading(r) => element(Some(r))
    | Complete(_) => React.null
    };

/**
Renders a Relude.AsyncResult if it's in an idle state (e.g. Init or Complete)
 */
let asyncResultIfIdle:
  'a 'e.
  (
    option(Belt.Result.t('a, 'e)) => React.element,
    Relude.AsyncResult.t('a, 'e)
  ) =>
  React.element
 =
  (element, result) =>
    switch (result) {
    | Init => element(None)
    | Loading => React.null
    | Reloading(_) => React.null
    | Complete(r) => element(Some(r))
    };