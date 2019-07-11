/**
Returns an element if the condition is true, otherwise React.null
*/
let ifTrue: (bool, React.element) => React.element =
  (condition, element) => if (condition) {element} else {React.null};

/**
Renders an element if the condition is true, otherwise React.null
*/
let ifTrueLazy: (bool, unit => React.element) => React.element =
  (condition, element) =>
    if (condition) {
      element();
    } else {
      React.null;
    };

/**
Renders an option with a None element or render fucntion for Some
 */
let option:
  'a.
  (option('a), React.element, 'a => React.element) => React.element
 =
  (opt, noneElement, someElement) =>
    switch (opt) {
    | None => noneElement
    | Some(a) => someElement(a)
    };

/**
Renders an option with render functions for None and Some
 */
let optionLazy:
  'a.
  (option('a), unit => React.element, 'a => React.element) => React.element
 =
  (opt, noneElement, someElement) =>
    switch (opt) {
    | None => noneElement()
    | Some(a) => someElement(a)
    };

/**
Renders an option with a render function for Some, otherwise React.null
 */
let optionIfSome: 'a. (option('a), 'a => React.element) => React.element =
  (opt, someElement) =>
    switch (opt) {
    | None => React.null
    | Some(a) => someElement(a)
    };

/**
Renders a Belt.Result.t('a, 'e), with render functions for both channels
 */
let result:
  'a 'e.
  (Belt.Result.t('a, 'e), 'a => React.element, 'e => React.element) =>
  React.element
 =
  (result, okElement, errorElement) =>
    switch (result) {
    | Ok(a) => okElement(a)
    | Error(e) => errorElement(e)
    };

/**
Renders a Belt.Result.t('a, 'e), with a render function for the Ok channel, otherwise React.null
 */
let resultIfOk:
  'a 'e.
  (Belt.Result.t('a, 'e), 'a => React.element) => React.element
 =
  (result, okElement) =>
    switch (result) {
    | Ok(a) => okElement(a)
    | Error(_) => React.null
    };

/**
Renders a Belt.Result.t('a, 'e), with a render function for the Error channel, otherwise React.null
 */
let resultIfError:
  'a 'e.
  (Belt.Result.t('a, 'e), 'e => React.element) => React.element
 =
  (result, errorElement) =>
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
    Relude.AsyncData.t('a),
    React.element,
    React.element,
    'a => React.element,
    'a => React.element
  ) =>
  React.element
 =
  (data, initElement, loadingElement, reloadingElement, completeElement) =>
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
    Relude.AsyncData.t('a),
    unit => React.element,
    unit => React.element,
    'a => React.element,
    'a => React.element
  ) =>
  React.element
 =
  (data, initElement, loadingElement, reloadingElement, completeElement) =>
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
  (Relude.AsyncData.t('a), React.element, 'a => React.element) =>
  React.element
 =
  (data, initOrLoadingElement, valueElement) =>
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
  (Relude.AsyncData.t('a), unit => React.element, 'a => React.element) =>
  React.element
 =
  (data, initOrLoadingElement, valueElement) =>
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
    Relude.AsyncResult.t('a, 'e),
    React.element,
    React.element,
    'a => React.element,
    'e => React.element,
    'a => React.element,
    'e => React.element
  ) =>
  React.element
 =
  (
    result,
    initElement,
    loadingElement,
    reloadingOkElement,
    reloadingErrorElement,
    completeOkElement,
    completeErrorElement,
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
    Relude.AsyncResult.t('a, 'e),
    unit => React.element,
    unit => React.element,
    'a => React.element,
    'e => React.element,
    'a => React.element,
    'e => React.element
  ) =>
  React.element
 =
  (
    result,
    initElement,
    loadingElement,
    reloadingOkElement,
    reloadingErrorElement,
    completeOkElement,
    completeErrorElement,
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
    Relude.AsyncResult.t('a, 'e),
    React.element,
    'a => React.element,
    'e => React.element
  ) =>
  React.element
 =
  (result, initOrLoadingElement, okElement, errorElement) =>
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
    Relude.AsyncResult.t('a, 'e),
    React.element,
    'a => React.element,
    'e => React.element
  ) =>
  React.element
 =
  (result, initOrLoadingElement, okElement, errorElement) =>
    switch (result) {
    | Init
    | Loading => initOrLoadingElement
    | Reloading(Ok(a))
    | Complete(Ok(a)) => okElement(a)
    | Reloading(Error(e))
    | Complete(Error(e)) => errorElement(e)
    };