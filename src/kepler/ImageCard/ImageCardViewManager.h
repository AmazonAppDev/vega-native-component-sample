// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <apmf/apmf_base.h>
#include "../AccessibilityDelegate.h"
#include <apmf/component.h>
#include <apmf/iface/com/amazon/apmf/ISubscription.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/AffineMatrix.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Color.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Size.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IGraphicsModule.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/TextAlignment.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ICommandDispatchDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IContext.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IContextDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayout.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/ILayoutDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistry.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IPropsRegistryBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAnimator.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAnimatorListener.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IInterpolator.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAccessibilityService.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageLoadListener.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/ITextView.h>
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

// Native view manager for the <ImageCard> JS component: a card showing a
// downloaded image above a block of description text, with a slide/fade
// transition each time a new image arrives.
//
// Events sent to JS: imageLoaded (each time a new image finishes loading).
//
// JS props (registered in registerProps): imageUrl, description, a11yLabel, prefetchUrls,
// slideDirection, transitionDuration, slideDistance, transitionEasing.
struct ImageCardViewManager
    : ApmfBase<ImageCardViewManager,
               IViewManager,
               IPropsDelegate,
               ILayoutDelegate,
               IContextDelegate> {
    ImageCardViewManager(View<IViewModule> viewModule,
                         View<IGraphicsModule> graphicsModule);

    Ptr<IView> makeView();
    void updateProps(View<IView> view, View<IJsObject> jsObject);
    Ptr<IPropsRegistry> registerProps(
        View<IPropsRegistryBuilder> propsRegistryBuilder);
    void updateLayout(View<IView> view, View<ILayout> newLayout);
    void updateContext(View<IContext> context);

private:
    Ptr<ITextLayout> getTextLayout(std::string text, float width,
                                   Color color = Color::WHITE,
                                   TextAlignment textAlign = TextAlignment::LEFT,
                                   bool isBold = false,
                                   float fontSize = 24.0f);
    void updateCardContent();
    void startTransitionAnimation();
    void onImageLoaded();
    void prefetchImages(View<IJsArray> urls);
    void applyTransitionProgress(float progress);
    Ptr<IInterpolator> makeTransitionInterpolator();

    Ptr<IGraphicsModule> graphicsModule_;
    Ptr<IViewModule> viewModule_;
    Ptr<IImageLoadListener> imageLoadListener_;
    Ptr<ITextView> textView_;
    Ptr<IImageView> imageView_;
    Ptr<IView> containerView_;
    Ptr<IContext> context_;
    Ptr<IAccessibilityService> accessibilityService_;
    Ptr<LabelAccessibilityDelegate> accessibilityDelegate_;
    Ptr<IImageLoadListener> prefetchListener_;
    std::vector<Ptr<IImageLoadRequest>> prefetchRequests_;
    // The animator, its listener and the listener subscription must outlive the
    // animation, so they are kept as members and replaced on each new transition.
    Ptr<IAnimator> transitionAnimator_;
    Ptr<IAnimatorListener> transitionListener_;
    Ptr<apmf::iface::com::amazon::apmf::ISubscription> transitionSubscription_;
    std::int64_t slideDirection_{0};
    std::int64_t transitionDurationMs_{500};
    std::int64_t slideDistance_{120};
    String transitionEasing_{"easeInOutCubic"};
    String imageSource_;
    String imageText_;
    Size contentSize_{350.0f, 350.0f};
    float imageCardBorderWidth_{2.0f};
};

}  // namespace nativeuicomponents
