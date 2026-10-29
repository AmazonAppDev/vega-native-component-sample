// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#pragma once

#include <vector>

#include <apmf/apmf_base.h>
#include "../AccessibilityDelegate.h"
#include <apmf/component.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IGraphicsModule.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ICommandDispatchDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IContext.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IContextDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayout.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayoutDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IJsObject.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IMountDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistry.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistryBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAccessibilityService.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IViewModule.h>
#include <apmf/ptr.h>
#include <apmf/string.h>
#include <apmf/string_view.h>

namespace nativeuicomponents {

using apmf::ApmfBase;
using apmf::Ptr;
using apmf::String;
using apmf::StringView;
using apmf::View;
using namespace apmf::iface::com::amazon::kepler::uitoolkit::graphics;
using namespace apmf::iface::com::amazon::kepler::uitoolkit::react;
using namespace apmf::iface::com::amazon::kepler::uitoolkit::view;

// Native view manager for the <ButtonContainer> JS component: a focusable
// container that draws its own border/fill, changes them on focus, and emits a
// "tapped" event to JS when an activation key (OK/Select/Enter) is released.
// JS children are mounted inside it (see mountChild/unmountChild).
struct ButtonContainerViewManager
    : ApmfBase<ButtonContainerViewManager,
               IViewManager,
               IContextDelegate,
               ILayoutDelegate,
               ICommandDispatchDelegate,
               IMountDelegate,
               IPropsDelegate> {
    ButtonContainerViewManager(View<IViewModule> viewModule,
                               View<IGraphicsModule> graphicsModule);
    Ptr<IView> makeView();
    void updateContext(View<IContext> context);
    void updateLayout(View<IView> view, View<ILayout> newLayout);
    void dispatchCommand(View<IView> view, StringView command, View<IJsArray> args);
    void mountChild(View<IView> const parentView, View<IView> const childView,
                    int32_t const index);
    void unmountChild(View<IView> const parentView, View<IView> const childView,
                      int32_t const index);
    void updateProps(View<IView> view, View<IJsObject> jsObject);
    Ptr<IPropsRegistry> registerProps(
        View<IPropsRegistryBuilder> propsRegistryBuilder);

private:
    Ptr<IGraphicsModule> graphicsModule_;
    Ptr<IViewModule> viewModule_;
    Ptr<IContext> context_;
    Ptr<IAccessibilityService> accessibilityService_;
    // One manager creates every button, so the delegates are per view and kept
    // alive here.
    std::vector<Ptr<LabelAccessibilityDelegate>> accessibilityDelegates_;
};

}  // namespace nativeuicomponents
