// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#pragma once

#include <apmf/apmf_base.h>
#include <apmf/iface/com/amazon/kepler/ucc/v1_0/common/LiveRegionMode.h>
#include <apmf/iface/com/amazon/kepler/ucc/v1_0/common/Role.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAccessibilityContext.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAccessibilityDelegate.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/ILiveTraitBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/ITrait.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/TraitType.h>
#include <apmf/ptr.h>
#include <apmf/string.h>

namespace nativeuicomponents {

namespace ucc = apmf::iface::com::amazon::kepler::ucc::v1_0::common;
using namespace apmf::iface::com::amazon::kepler::uitoolkit::view;

// Publishes a view's accessibility information so screen readers such as
// VoiceView can announce it. Native views publish nothing by default, so each
// view that should be spoken installs one of these through
// IAccessibilityService::setDelegate().
//
// - The label is spoken when the view takes focus (or, for a live region, when
//   the label changes).
// - isButton marks the view as a button so the role is announced too.
// - isLiveRegion makes a polite live region: a change to the label is
//   announced without the view needing focus.
//
// After changing the label, call IAccessibilityService::invalidate(view) so the
// delegate is asked to rebuild.
struct LabelAccessibilityDelegate
    : apmf::ApmfBase<LabelAccessibilityDelegate, IAccessibilityDelegate> {
    LabelAccessibilityDelegate(bool isButton, bool isLiveRegion)
        : isButton_(isButton), isLiveRegion_(isLiveRegion) {}

    void setLabel(apmf::String label) { label_ = label; }

    void build(apmf::View<IAccessibilityContext> const context,
               apmf::View<IAccessibilityDelegate> const defaultDelegate) {
        // The default implementation adds this view's node to the
        // accessibility tree; it does not emit any information itself.
        defaultDelegate->build(context, defaultDelegate);
        context->setDescription(label_);
        if (isButton_) {
            context->setRole(ucc::Role::BUTTON);
        }
        if (isLiveRegion_) {
            auto const liveTrait = context->makeLiveTraitBuilder();
            liveTrait->mode(ucc::LiveRegionMode::POLITE);
            context->setTrait(liveTrait->build());
        }
    }

    void clear(apmf::View<IAccessibilityContext> const context,
               apmf::View<IAccessibilityDelegate> const defaultDelegate) {
        defaultDelegate->clear(context, defaultDelegate);
        if (isLiveRegion_) {
            context->clearTrait(TraitType::LIVE);
        }
    }

private:
    bool isButton_;
    bool isLiveRegion_;
    apmf::String label_;
};

}  // namespace nativeuicomponents
