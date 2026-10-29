// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include "ButtonContainerViewManager.h"

#include <apmf/fn.h>
#include <apmf/iface/com/amazon/kepler/input/events/KeyCode.h>
#include <apmf/iface/com/amazon/kepler/input/events/KeyEvent.h>
#include <apmf/iface/com/amazon/kepler/input/events/KeyState.h>
#include <apmf/iface/com/amazon/kepler/input/events/KeyStateEvent.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Color.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Rect.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IEventEmitter.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IFrame.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IFocusChangeListener.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IFocusManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IFocusState.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IKeyEventListener.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IUIKeyEvent.h>
#include <apmf/log.h>

APMF_DEFINE_LOGGER(ButtonContainerLogger,
                   "com.amazondeveloper.nativecomponentexample.buttoncontainer");

namespace nativeuicomponents {

using namespace apmf::iface::com::amazon::kepler::input::events;
using namespace apmf::log_levels;

ButtonContainerViewManager::ButtonContainerViewManager(
    View<IViewModule> viewModule, View<IGraphicsModule> graphicsModule)
    : viewModule_(viewModule), graphicsModule_(graphicsModule) {}

// Creates the native view once per JS component instance and installs its
// unfocused look plus the key and focus listeners.
Ptr<IView> ButtonContainerViewManager::makeView() {
    auto const viewOpacityContainer = viewModule_->makeView();
    auto const unfocusedBorderColor =
        Color{82.0f / 255, 82.0f / 255, 91.0f / 255, 1.0f};
    viewOpacityContainer->setBackgroundColor(
        Color{34.0f / 255, 34.0f / 255, 34.0f / 255, 0.96f});
    viewOpacityContainer->setBorderWidth({1.0f, 1.0f, 1.0f, 1.0f});
    viewOpacityContainer->setBorderColor({unfocusedBorderColor,
                                          unfocusedBorderColor,
                                          unfocusedBorderColor,
                                          unfocusedBorderColor});
    viewOpacityContainer->setBorderRadius({16.0f, 16.0f, 16.0f, 16.0f});
    viewOpacityContainer->setOpacity(1.0f);

    // Without this the D-pad can never land on the view.
    auto focusState = viewModule_->makeFocusManager()->getFocusState();
    focusState->setFocusable(viewOpacityContainer, true);

    // Activation is reported on key UP (not down) so a held key fires once.
    // Several key codes are accepted because remotes and keyboards differ.
    viewOpacityContainer->addKeyEventListener(
        apmf::Fn1([self = this](View<IView> view, View<IUIKeyEvent> keyEvent) {
            auto keyStateEvent = keyEvent->getKeyStateEvent();
            auto const pressedKey = keyStateEvent->keyCode().value;
            auto const isActivationKey =
                pressedKey == KeyCode::ENTER.value ||
                pressedKey == KeyCode::KPENTER.value ||
                pressedKey == KeyCode::OK.value ||
                pressedKey == KeyCode::SELECT.value ||
                pressedKey == KeyCode::APP_SELECT.value ||
                pressedKey == KeyCode::BUTTON_LEFT_MENU_SELECT.value;
            APMF_LOG(ButtonContainerLogger, INFO)
                << "onKey event triggered character :: "
                << pressedKey;

            if (keyStateEvent->state() == KeyState::UP && isActivationKey) {
                auto const eventEmitter = self->context_->getEventEmitter();
                eventEmitter->emit(view, "tapped");
                APMF_LOG(ButtonContainerLogger, INFO) << "emitted event";
            }
        }));

    // Focus visuals are drawn natively: a thicker, brighter border and a lighter
    // fill while focused, restored on blur. The border width change is a second
    // cue so focus is not conveyed by colour alone.
    auto const focusManager = viewModule_->makeFocusManager();
    focusManager->addFocusChangeListener(
        viewOpacityContainer,
        apmf::Fn([self = this](apmf::method::onFocusChanged, View<IView> view,
                               bool hasFocus) {
            if (hasFocus) {
                APMF_LOG(ButtonContainerLogger, INFO) << "Button focus gained";
                auto const focusedBorderColor =
                    Color{0.0f, 212.0f / 255, 1.0f, 1.0f};
                view->setBorderWidth({3.0f, 3.0f, 3.0f, 3.0f});
                view->setBorderColor({focusedBorderColor,
                                      focusedBorderColor,
                                      focusedBorderColor,
                                      focusedBorderColor});
                view->setBackgroundColor(
                    Color{58.0f / 255, 58.0f / 255, 64.0f / 255, 1.0f});
                view->setOpacity(1.0f);
            } else {
                APMF_LOG(ButtonContainerLogger, INFO) << "Button focus lost";
                auto const unfocusedBorderColor =
                    Color{82.0f / 255, 82.0f / 255, 91.0f / 255, 1.0f};
                view->setBorderWidth({1.0f, 1.0f, 1.0f, 1.0f});
                view->setBorderColor({unfocusedBorderColor,
                                      unfocusedBorderColor,
                                      unfocusedBorderColor,
                                      unfocusedBorderColor});
                view->setBackgroundColor(
                    Color{34.0f / 255, 34.0f / 255, 34.0f / 255, 0.96f});
                view->setOpacity(1.0f);
            }
        }));
    return viewOpacityContainer;
}

// Called with only the props that changed. a11yLabel is what a screen reader
// speaks when this button takes focus. Native views are invisible to screen
// readers unless they publish their own accessibility information, so each
// button gets its own delegate carrying its label.
void ButtonContainerViewManager::updateProps(View<IView> view,
                                             View<IJsObject> newPropsUpdates) {
    for (auto const& key : newPropsUpdates->getAllKeys()) {
        if (key == "a11yLabel") {
            if (!accessibilityService_) {
                accessibilityService_ = viewModule_->makeAccessibilityService();
            }
            auto const delegate = Ptr<LabelAccessibilityDelegate>::Make(
                /*isButton=*/true, /*isLiveRegion=*/false);
            delegate->setLabel(newPropsUpdates->getString("a11yLabel"));
            accessibilityDelegates_.push_back(delegate);
            accessibilityService_->setDelegate(view, delegate);
            accessibilityService_->invalidate(view);
        }
    }
}

// Declares the props this component accepts. Keep in sync with
// validAttributes in ButtonContainerSpec.ts.
Ptr<IPropsRegistry> ButtonContainerViewManager::registerProps(
    View<IPropsRegistryBuilder> propsRegistryBuilder) {
    propsRegistryBuilder->stringPropWithDefault("a11yLabel", "");
    return propsRegistryBuilder->build();
}

// The context gives access to the event emitter used to send "tapped" to JS.
void ButtonContainerViewManager::updateContext(View<IContext> context) {
    APMF_LOG(ButtonContainerLogger, INFO) << "Triggered updateContext()";
    context_ = context;
}

// Called when React's layout (Yoga) computes a new frame; the native view is
// simply resized to match.
void ButtonContainerViewManager::updateLayout(View<IView> view,
                                              View<ILayout> newLayout) {
    APMF_LOG(ButtonContainerLogger, INFO) << "Triggered updateLayout()";
    auto size = newLayout->getContentFrame()->getSize();
    APMF_LOG(ButtonContainerLogger, INFO) << "Size width: " << size.width;
    APMF_LOG(ButtonContainerLogger, INFO) << "Size height: " << size.height;
    view->setSize(size);
}

void ButtonContainerViewManager::dispatchCommand(View<IView> view,
                                                 StringView command,
                                                 View<IJsArray> args) {
    APMF_LOG(ButtonContainerLogger, INFO) << "Triggered dispatchCommand()";

    if (command == "changeCommand") {
        APMF_LOG(ButtonContainerLogger, INFO)
            << "dispatchCommand command: " << command;
    }
}

// JS children (the chevron) are attached to this native view in order.
void ButtonContainerViewManager::mountChild(View<IView> const parentView,
                                            View<IView> const childView,
                                            int32_t const index) {
    APMF_LOG(ButtonContainerLogger, INFO)
        << "MountChild is called at index " << index;
    parentView->addChildAtIndex(childView, index);
}

void ButtonContainerViewManager::unmountChild(View<IView> const parentView,
                                              View<IView> const childView,
                                              int32_t const index) {
    APMF_LOG(ButtonContainerLogger, INFO)
        << "UnmountChild is called at " << index;
    parentView->removeChildAtIndex(index);
}

}  // namespace nativeuicomponents
