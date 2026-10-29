// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include "ViewManagerFactory.h"
#include "ButtonContainer/ButtonContainerViewManager.h"
#include "ImageCard/ImageCardViewManager.h"

#include <apmf/component.h>
#include <apmf/iface/com/amazon/apmf/InvalidArgumentError.h>

namespace nativeuicomponents {

// Maps the component name registered in JS (see the *Spec.ts files) to its C++
// view manager. Add new native components here.
Ptr<IViewManager> ViewManagerFactory::makeViewManager(StringView name) {
    auto const viewModule = apmf::GetProcessObject()
                                ->getComponent("/com.amazon.kepler.uitoolkit.view")
                                .TryQueryInterface<IViewModule>();
    auto const graphicsModule = apmf::GetProcessObject()
                                    ->getComponent("/com.amazon.kepler.uitoolkit.graphics")
                                    .TryQueryInterface<IGraphicsModule>();

    if (name == "ImageCard") {
        return Ptr<ImageCardViewManager>::Make(viewModule, graphicsModule);
    }
    if (name == "ButtonContainer") {
        return Ptr<ButtonContainerViewManager>::Make(viewModule, graphicsModule);
    }
    throw apmf::InvalidArgumentError("Cannot make UI component");
}

}  // namespace nativeuicomponents

// Publishes the factory under the component path declared for this package.
APMF_COMPONENT("/com.amazondeveloper.nativecomponentexample",
               nativeuicomponents::ViewManagerFactory);
