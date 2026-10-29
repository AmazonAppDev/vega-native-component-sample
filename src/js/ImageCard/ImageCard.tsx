// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

import React from 'react';
import {ViewProps} from 'react-native';
import ImageCardNative from './ImageCardSpec';

// Thin typed wrapper around the native ImageCard view. Extra props (style,
// transition tuning) are passed straight through to the native component.
export interface ImageCardProps extends ViewProps {
  imageUrl?: string;
  description?: string;
  /** Spoken by screen readers; changing it is announced (the card is a live region). */
  a11yLabel?: string;
  /** Called each time a new image has finished loading. */
  onImageLoaded?: () => void;
  /** Image URLs to download ahead of time so later transitions hit the cache. */
  prefetchUrls?: string[];
  /** -1 slides in from the left, 1 from the right, 0 fades only. */
  slideDirection?: number;
  /** Transition length in milliseconds. */
  transitionDuration?: number;
  /** Slide distance in pixels. */
  slideDistance?: number;
  /** linear | easeInCubic | easeOutCubic | easeInOutCubic | easeOutElastic */
  transitionEasing?: string;
}

export const ImageCard: React.FunctionComponent<ImageCardProps> = (
  props: ImageCardProps,
) => {
  const {imageUrl, description, slideDirection, ...rest} = props;

  return (
    <ImageCardNative
      imageUrl={imageUrl}
      description={description}
      slideDirection={slideDirection}
      {...rest}
    />
  );
};
