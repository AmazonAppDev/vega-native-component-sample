// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

// JS side of the native ImageCard: registers which props are forwarded to the
// C++ view manager. A prop must be listed in validAttributes here and in
// registerProps() in ImageCardViewManager.cpp to reach native code.
// @ts-ignore
import register from '@amazon-devices/react-native-kepler/Libraries/Utilities/registerGeneratedViewConfig';
import {ViewProps} from 'react-native';
import {
  BubblingEventHandler,
  HostComponent,
} from 'react-native-tscodegen-types';

type ImageLoadedEvent = {};

interface NativeProps extends ViewProps {
  onImageLoaded?: BubblingEventHandler<ImageLoadedEvent>;
  imageUrl?: string;
  description?: string;
  a11yLabel?: string;
  prefetchUrls?: string[];
  slideDirection?: number;
  transitionDuration?: number;
  slideDistance?: number;
  transitionEasing?: string;
}

const ImageCardComponent = 'ImageCard';

// The native side emits "imageLoaded". Depending on the runtime path it can
// arrive under several names, so each is mapped to the onImageLoaded prop.
const imageLoadedEvent = {
  phasedRegistrationNames: {
    bubbled: 'onImageLoaded',
    captured: 'onImageLoadedCapture',
  },
};

const viewConfig = {
  bubblingEventTypes: {
    topImageLoaded: imageLoadedEvent,
    imageLoaded: imageLoadedEvent,
    onImageLoaded: imageLoadedEvent,
    topOnImageLoaded: imageLoadedEvent,
  },
  validAttributes: {
    imageUrl: true,
    description: true,
    a11yLabel: true,
    prefetchUrls: true,
    slideDirection: true,
    transitionDuration: true,
    slideDistance: true,
    transitionEasing: true,
  },
};

register(ImageCardComponent, {
  uiViewClassName: 'ImageCard',
  ...viewConfig,
});

export default ImageCardComponent as unknown as HostComponent<NativeProps>;
