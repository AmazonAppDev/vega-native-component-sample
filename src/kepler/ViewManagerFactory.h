// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#pragma once

#include <apmf/apmf_base.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IViewManagerFactory.h>
#include <apmf/process.h>
#include <apmf/ptr.h>
#include <apmf/string.h>
#include <apmf/string_view.h>

namespace nativeuicomponents {

using apmf::ApmfBase;
using apmf::Ptr;
using apmf::String;
using apmf::StringView;
using apmf::View;
using namespace apmf::iface::com::amazon::kepler::uitoolkit::react;

// Entry point the Kepler UI toolkit calls to create a view manager for each
// native component name referenced from JS.
struct ViewManagerFactory : ApmfBase<ViewManagerFactory, IViewManagerFactory> {
    Ptr<IViewManager> makeViewManager(StringView name);
};

}  // namespace nativeuicomponents
