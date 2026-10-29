// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

#include "ImageCardViewManager.h"

#include <algorithm>

#include <apmf/array.h>
#include <apmf/iface/com/amazon/apmf/MonotonicDuration.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAnimationValue.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAnimationValueBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IAnimatorBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IInterpolator.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IInterpolatorModule.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/AnimationValueType.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/FontSlant.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/FontWeight.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/FontWidth.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IAttributedText.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IFontStyle.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IParagraphStyle.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/IParagraphStyleBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ISpan.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ITextLayout.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ITextLayoutBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ITextStyle.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ITextStyleBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/ITextStyleSpan.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Point.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Rect.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/Size.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/graphics/TextDecoration.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IEventEmitter.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IFrame.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IJsArray.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/react/IJsObject.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/BorderColor.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/Fit.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IFocusManager.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IFocusState.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageLoadRequest.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageLoader.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageOptions.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageOptionsBuilder.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IImageView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/ITextView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/IView.h>
#include <apmf/iface/com/amazon/kepler/uitoolkit/view/ImageLoadError.h>
#include <apmf/log.h>

APMF_DEFINE_LOGGER(
    ImageCardLogger,
    "com.amazondeveloper.nativecomponentexample.ImageCardViewManager");

namespace nativeuicomponents {

using namespace apmf::log_levels;

template <typename T>
using Array = apmf::Array<T>;

// Receives the result of the asynchronous image download. On success the image
// is handed to the image view and onLoaded is invoked, which is what starts the
// card transition (so the animation reveals the new image, not the old one).
struct ImageLoadListener : ApmfBase<ImageLoadListener, IImageLoadListener> {
    ImageLoadListener(View<IImageView> imageView, String uri,
                      std::function<void()> onLoaded = nullptr)
        : imageView_(imageView), uri_(uri), onLoaded_(std::move(onLoaded)) {}

    void onStart(View<IImageLoadRequest> const request) {
        APMF_LOG(ImageCardLogger, INFO)
            << "Image download is started. Uri - " << uri_;
    }
    void onProgress(View<IImageLoadRequest> const request, float const progress) {}
    void onSuccess(View<IImageLoadRequest> const request, View<IImage> const image) {
        APMF_LOG(ImageCardLogger, INFO)
            << "Image download is  successful. Uri - " << uri_;
        imageView_->setImage(image);
        if (onLoaded_) {
            onLoaded_();
        }
    }
    void onFailure(View<IImageLoadRequest> const request,
                   ImageLoadError const& error) {
        APMF_LOG(ImageCardLogger, INFO)
            << "Image download is  failed. Uri - " << uri_;
    }
    void onCanceled(View<IImageLoadRequest> const request) {
        APMF_LOG(ImageCardLogger, INFO)
            << "Image download is Cancelled. Uri - " << uri_;
    }
    void onPrefetchComplete(View<IImageLoadRequest> const request) {
        APMF_LOG(ImageCardLogger, INFO)
            << "Image download is Prefetch. Uri - " << uri_;
    }

    Ptr<IImageView> imageView_;
    String uri_;
    std::function<void()> onLoaded_;
};

// Listener for prefetch requests. Prefetching only warms the image cache, so
// the result is just logged; the real load later hits the cache.
struct PrefetchListener : ApmfBase<PrefetchListener, IImageLoadListener> {
    void onStart(View<IImageLoadRequest> const request) {}
    void onProgress(View<IImageLoadRequest> const request, float const progress) {}
    void onSuccess(View<IImageLoadRequest> const request, View<IImage> const image) {}
    void onFailure(View<IImageLoadRequest> const request,
                   ImageLoadError const& error) {
        APMF_LOG(ImageCardLogger, INFO) << "Image prefetch failed";
    }
    void onCanceled(View<IImageLoadRequest> const request) {}
    void onPrefetchComplete(View<IImageLoadRequest> const request) {
        APMF_LOG(ImageCardLogger, INFO) << "Image prefetch complete";
    }
};

namespace {

// The animator only produces values over time; it does not change any view.
// This listener reads the current 0..1 value on every frame (onUpdate) and
// forwards it so the view manager can apply it to the card.
struct TransitionAnimatorListener
    : ApmfBase<TransitionAnimatorListener, IAnimatorListener> {
    explicit TransitionAnimatorListener(std::function<void(float)> onProgress)
        : onProgress_(std::move(onProgress)) {}

    void onStart(View<IAnimator> const animator) {}
    void onEnd(View<IAnimator> const animator) {}
    void onCancel(View<IAnimator> const animator) {}
    void onPause(View<IAnimator> const animator) {}
    void onResume(View<IAnimator> const animator) {}
    void onUpdate(View<IAnimator> const animator) {
        auto const value = animator->getCurrentAnimationValue();
        if (value &&
            value->getValueType().value == AnimationValueType::FLOAT.value) {
            onProgress_(*value->getFloatValue());
        }
    }

    std::function<void(float)> onProgress_;
};

}  // namespace

ImageCardViewManager::ImageCardViewManager(
    View<IViewModule> viewModule, View<IGraphicsModule> graphicsModule)
    : viewModule_(viewModule), graphicsModule_(graphicsModule) {}

// Builds the native view tree once: a bordered container holding an image view
// and a text view. Sizes and offsets are set later in updateCardContent().
Ptr<IView> ImageCardViewManager::makeView() {
    APMF_LOG(ImageCardLogger, INFO) << "makeView";

    imageView_ = viewModule_->makeImageView();
    imageView_->setFit(Fit::CENTER);

    imageLoadListener_ = Ptr<ImageLoadListener>::Make(
        imageView_, imageSource_, [this] { onImageLoaded(); });

    textView_ = viewModule_->makeTextView(getTextLayout(imageText_, 350));

    auto const containerView = viewModule_->makeView();
    auto const cardBorderColor =
        Color{63.0f / 255, 63.0f / 255, 70.0f / 255, 1.0f};
    containerView->setBackgroundColor(
        Color{34.0f / 255, 34.0f / 255, 34.0f / 255, 0.96f});
    containerView->setBorderWidth({imageCardBorderWidth_, imageCardBorderWidth_,
                                   imageCardBorderWidth_, imageCardBorderWidth_});
    containerView->setBorderColor({cardBorderColor,
                                   cardBorderColor,
                                   cardBorderColor,
                                   cardBorderColor});
    containerView->setBorderRadius({22.0f, 22.0f, 22.0f, 22.0f});

    containerView->addChild(imageView_);
    containerView->addChild(textView_);

    auto focusState = viewModule_->makeFocusManager()->getFocusState();
    // The card is display-only; the arrow buttons are the focusable elements.
    focusState->setFocusable(containerView, false);
    containerView_ = containerView;

    // The card never takes focus, so it is published as a polite live region:
    // when its label changes, the screen reader speaks the new card.
    accessibilityDelegate_ = Ptr<LabelAccessibilityDelegate>::Make(
        /*isButton=*/false, /*isLiveRegion=*/true);
    accessibilityService_ = viewModule_->makeAccessibilityService();
    accessibilityService_->setDelegate(containerView, accessibilityDelegate_);
    return containerView;
}

// Called with only the props that changed. A new imageUrl kicks off a download
// and the transition starts when it completes; slideDirection and the tuning
// props are just stored for the next transition.
void ImageCardViewManager::updateProps(View<IView> view,
                                       View<IJsObject> newPropsUpdates) {
    APMF_LOG(ImageCardLogger, INFO) << "updateProps";

    Ptr<IJsArray> prefetchUrls;
    for (auto const& key : newPropsUpdates->getAllKeys()) {
        if (key == "prefetchUrls") {
            prefetchUrls = newPropsUpdates->getArray("prefetchUrls");
        } else if (key == "imageUrl") {
            imageSource_ = newPropsUpdates->getString("imageUrl");
            APMF_LOG(ImageCardLogger, INFO)
                << "ImageUIComponentViewManager:: updateProps :: imageSource "
                << imageSource_;
            imageLoadListener_ = Ptr<ImageLoadListener>::Make(
                imageView_, imageSource_, [this] { onImageLoaded(); });
            auto const imageLoader = viewModule_->makeImageLoader();
            imageLoader->load(imageSource_, imageLoadListener_, nullptr);
        } else if (key == "a11yLabel") {
            accessibilityDelegate_->setLabel(newPropsUpdates->getString("a11yLabel"));
            accessibilityService_->invalidate(view);
        } else if (key == "slideDirection") {
            slideDirection_ = newPropsUpdates->getInt("slideDirection");
        } else if (key == "transitionDuration") {
            transitionDurationMs_ = newPropsUpdates->getInt("transitionDuration");
        } else if (key == "slideDistance") {
            slideDistance_ = newPropsUpdates->getInt("slideDistance");
        } else if (key == "transitionEasing") {
            transitionEasing_ = newPropsUpdates->getString("transitionEasing");
        } else if (key == "description") {
            imageText_ = newPropsUpdates->getString("description");
            APMF_LOG(ImageCardLogger, INFO)
                << "ImageUIComponentViewManager:: updateProps :: description "
                << imageText_;
        }
    }
    // Prefetch after the visible image's load has been issued so the image on
    // screen is never queued behind the warm-up downloads.
    if (prefetchUrls) {
        prefetchImages(prefetchUrls);
    }
    updateCardContent();
    APMF_LOG(ImageCardLogger, INFO) << "updateProps Completed";
}

// Declares the props this component accepts, with native defaults. Keep in sync
// with validAttributes in ImageCardSpec.ts.
Ptr<IPropsRegistry> ImageCardViewManager::registerProps(
    View<IPropsRegistryBuilder> propsRegistryBuilder) {
    APMF_LOG(ImageCardLogger, INFO) << "registerProps";

    propsRegistryBuilder->stringPropWithDefault("imageUrl", imageSource_);
    propsRegistryBuilder->stringPropWithDefault("description", imageText_);
    propsRegistryBuilder->stringPropWithDefault("a11yLabel", "");
    propsRegistryBuilder->arrayProp("prefetchUrls");
    propsRegistryBuilder->intPropWithDefault("slideDirection", slideDirection_);
    propsRegistryBuilder->intPropWithDefault("transitionDuration",
                                             transitionDurationMs_);
    propsRegistryBuilder->intPropWithDefault("slideDistance", slideDistance_);
    propsRegistryBuilder->stringPropWithDefault("transitionEasing",
                                                transitionEasing_);
    return propsRegistryBuilder->build();
}

void ImageCardViewManager::updateLayout(View<IView> view,
                                        View<ILayout> newLayout) {
    APMF_LOG(ImageCardLogger, INFO) << "Triggered updateLayout()";

    auto size = newLayout->getContentFrame()->getSize();
    APMF_LOG(ImageCardLogger, INFO) << "Size width: " << size.width;
    APMF_LOG(ImageCardLogger, INFO) << "Size height: " << size.height;
    view->setSize(size);
    contentSize_ = size;
    updateCardContent();
}

// Lays out the card contents inside the container: the image takes the top
// half, and the description sits below it, inset by the card padding.
void ImageCardViewManager::updateCardContent() {
    if (!imageView_ || !textView_) {
        return;
    }

    auto const horizontalPadding = 20.0f;
    auto const verticalPadding = 18.0f;
    auto const contentWidth =
        std::max(1.0f, contentSize_.width - horizontalPadding * 2);
    auto const imageHeight =
        std::max(1.0f, static_cast<float>(contentSize_.height * 0.5f));
    auto const textTop = imageHeight + verticalPadding * 1.65f;
    auto const textHeight = std::max(
        1.0f, contentSize_.height - textTop - verticalPadding * 1.5f);

    imageView_->setOffset({horizontalPadding, verticalPadding});
    imageView_->setSize({contentWidth, imageHeight});

    textView_->setOffset({horizontalPadding, textTop});
    textView_->setSize({contentWidth, textHeight});
    textView_->updateTextLayout(
        getTextLayout(imageText_, contentWidth,
                      Color{229.0f / 255, 229.0f / 255, 231.0f / 255, 1.0f},
                      TextAlignment::LEFT, false, 24.0f));
}

// The context gives access to the event emitter used to send events to JS.
void ImageCardViewManager::updateContext(View<IContext> context) {
    context_ = context;
}

// Runs when a new image has finished loading: play the reveal animation and
// tell JS, which uses the first event as its "fully drawn" launch marker.
void ImageCardViewManager::onImageLoaded() {
    startTransitionAnimation();
    if (context_ && containerView_) {
        context_->getEventEmitter()->emit(containerView_, "imageLoaded");
    }
}

// Builds the text layout for the description: wrapped to the given width,
// limited to four lines and truncated with an ellipsis.
Ptr<ITextLayout> ImageCardViewManager::getTextLayout(std::string text,
                                                     float width, Color color,
                                                     TextAlignment textAlign,
                                                     bool isBold,
                                                     float fontSize) {
    auto paragraphStyleBuilder = graphicsModule_->makeParagraphStyleBuilder();
    paragraphStyleBuilder->maxLines(4);
    paragraphStyleBuilder->ellipsis("...");
    paragraphStyleBuilder->textAlignment(textAlign);

    auto const boldFontStyle = graphicsModule_->makeFontStyle(
        (isBold ? FontWeight::EXTRA_BOLD : FontWeight::NORMAL),
        FontWidth::NORMAL, FontSlant::UPRIGHT);

    auto textStyleBuilder = graphicsModule_->makeTextStyleBuilder();
    textStyleBuilder->color(color);
    textStyleBuilder->fontStyle(boldFontStyle);
    textStyleBuilder->fontSize(fontSize);
    Array<Ptr<ISpan>> spansArray;
    auto const attributedText = graphicsModule_->makeAttributedText(text, spansArray);

    auto textLayoutBuilder =
        graphicsModule_->makeTextLayoutBuilder(width, attributedText);
    textLayoutBuilder->textStyle(textStyleBuilder->build());
    textLayoutBuilder->paragraphStyle(paragraphStyleBuilder->build());

    auto const textLayout = textLayoutBuilder->build();
    return textLayout;
}

// Downloads every URL into the image cache ahead of time. Without this, the
// first visit to each card downloads its poster while the transition is
// running, which shows up as a stuttering slide.
void ImageCardViewManager::prefetchImages(View<IJsArray> urls) {
    if (!prefetchListener_) {
        prefetchListener_ = Ptr<PrefetchListener>::Make();
    }
    auto const imageLoader = viewModule_->makeImageLoader();
    prefetchRequests_.clear();
    for (int32_t i = 0; i < urls->getElementCount(); ++i) {
        prefetchRequests_.push_back(
            imageLoader->prefetch(urls->getString(i), prefetchListener_, nullptr));
    }
}

// Maps animation progress t (0 -> 1) onto the card: opacity follows t, and the
// card slides horizontally from slideDirection * slideDistance pixels away to
// its resting position. The slide uses the transformation matrix rather than
// the view offset because layout owns the offset and would overwrite it.
void ImageCardViewManager::applyTransitionProgress(float progress) {
    if (!containerView_) {
        return;
    }
    auto const clamped = std::min(1.0f, std::max(0.0f, progress));
    auto const translateX =
        static_cast<float>(slideDirection_) * (1.0f - clamped) *
        static_cast<float>(slideDistance_);
    containerView_->setOpacity(clamped);
    containerView_->setTransformationMatrix(
        AffineMatrix{{1.0f, 0.0f, translateX, 0.0f, 1.0f, 0.0f}});
}

// Chooses the easing curve from the transitionEasing prop (defaults to
// ease-in-out cubic for unrecognised names).
Ptr<IInterpolator> ImageCardViewManager::makeTransitionInterpolator() {
    auto const interpolators = viewModule_->makeInterpolatorModule();
    if (transitionEasing_ == "linear") {
        return interpolators->makeLinearInterpolator();
    }
    if (transitionEasing_ == "easeInCubic") {
        return interpolators->makeEaseInCubicInterpolator();
    }
    if (transitionEasing_ == "easeOutCubic") {
        return interpolators->makeEaseOutCubicInterpolator();
    }
    if (transitionEasing_ == "easeOutElastic") {
        return interpolators->makeEaseOutElasticInterpolator();
    }
    return interpolators->makeEaseInOutCubicInterpolator();
}

// Runs one 0 -> 1 transition. Any animation still in flight is cancelled first
// so rapid button presses restart cleanly instead of fighting each other, and
// the card is reset to progress 0 (transparent, offset) before starting so it
// never flashes at full opacity.
void ImageCardViewManager::startTransitionAnimation() {
    if (!containerView_) {
        return;
    }
    if (transitionAnimator_) {
        transitionAnimator_->cancel();
    }
    if (transitionSubscription_) {
        transitionSubscription_->unsubscribe();
    }

    applyTransitionProgress(0.0f);

    auto const makeFloatValue = [this](float value) {
        auto const builder = viewModule_->makeAnimationValueBuilder();
        builder->floatValue(value);
        return builder->build();
    };
    auto const start = makeFloatValue(0.0f);
    auto const end = makeFloatValue(1.0f);

    auto const animatorBuilder = viewModule_->makeAnimatorBuilder();
    animatorBuilder->duration(apmf::iface::com::amazon::apmf::MonotonicDuration{
        transitionDurationMs_ / 1000,
        static_cast<std::int32_t>((transitionDurationMs_ % 1000) * 1000000)});
    animatorBuilder->interpolator(makeTransitionInterpolator());
    animatorBuilder->animationBounds(start, end);
    transitionAnimator_ = animatorBuilder->build();

    transitionListener_ = Ptr<TransitionAnimatorListener>::Make(
        [this](float progress) { applyTransitionProgress(progress); });
    transitionSubscription_ =
        transitionAnimator_->addAnimationListener(transitionListener_);
    transitionAnimator_->start();
}

}  // namespace nativeuicomponents
