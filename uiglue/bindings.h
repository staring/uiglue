//===-- Functions for declaring bindings ----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef BINDINGS_H
#define BINDINGS_H

#include "view_model_ref.h"

#include "curt/api_params.h"
#include "curt/fwd_windows.h"

#include <memory>
#include <string>

namespace uiglue {

class View;
class BindingHandlerCache;

void applyBindingsInner(std::unique_ptr<ViewModelRef> vmRef, HWND view);

class BindingDeclaration {
  std::unique_ptr<View> viewData;
  HWND handle;

public:
  BindingDeclaration(HWND handle, BindingHandlerCache cache);
  ~BindingDeclaration();

  BindingDeclaration& operator()(
    int controlId,
    std::string binding,
    std::string value
  );
};


BindingDeclaration declareBindings(curt::HandleOr<HWND> view, BindingHandlerCache);

BindingHandlerCache defaultBindingHandlers();

template<class ViewModel>
void applyBindings(ViewModel& vm, curt::HandleOr<HWND> view) {
  auto ref = std::make_unique<ViewModelRefImpl<ViewModel>>(vm);
  applyBindingsInner(std::move(ref), view);
}

void detachViewModel(curt::HandleOr<HWND> view);

} // end namespace uiglue

#endif
