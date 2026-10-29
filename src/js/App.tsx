// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

/**
 * Sample screen: a title, a native ImageCard in the middle, and a native
 * focusable arrow button on each side that steps through the catalog.
 * ImageCard and Button are backed by C++ view managers in src/kepler.
 */
import React, {useState} from 'react';
import {StyleSheet, Text, View} from 'react-native';
import {Button} from './ButtonContainer/ButtonContainer';
import {ImageCard} from './ImageCard/ImageCard';
import {catalog as images} from './catalog';
import {cardTransition, colors, SAFE_ZONE_INSET, tvTypography} from './theme';
import {useFullyDrawnMarker} from './useFullyDrawnMarker';

// Constant so the same array is passed on every render; the native card
// prefetches these once up front so later transitions do not wait on downloads.
const prefetchUrls = images.map((item) => item.url);

export const App = () => {
  const [index, setIndex] = useState<number>(0);
  // Launch is complete once the first poster has loaded, not at first render.
  const reportContentLoaded = useFullyDrawnMarker();
  // -1 = user went backward, 1 = forward. Passed to the native card so it
  // slides in from the matching side; 0 (initial) is fade-only.
  const [slideDirection, setSlideDirection] = useState<number>(0);

  // Wraps around at both ends so the carousel never dead-ends.
  const nextIndex = (isNext: boolean) => {
    return (index + (isNext ? 1 : -1) + images.length) % images.length;
  };

  const handleTapped = (isNext: boolean) => {
    setSlideDirection(isNext ? 1 : -1);
    setIndex(nextIndex(isNext));
  };

  const selectedImage = images[index];

  // What a screen reader says when the card changes: title, genre and year.
  // The description is left out because live-region text should stay short, and
  // symbols such as the bullet and slash would otherwise be read aloud.
  const cardLabel = `${selectedImage.title}. ${selectedImage.meta.replace(
    /\s*[•/]\s*/g,
    ', ',
  )}`;

  return (
    <View style={styles.background}>
      <View style={styles.backgroundGlowTop} />
      <View style={styles.backgroundGlowBottom} />
      <View style={styles.mainContent}>
        <View style={styles.headerPanel}>
          <Text style={styles.eyebrow} numberOfLines={1}>
            Vega native component example
          </Text>
          {/* role="heading" lets VoiceView present the title on screen entry. */}
          <Text
            role="heading"
            style={styles.headerText}
            numberOfLines={2}
            ellipsizeMode="tail">
            {selectedImage.title}
          </Text>
          <Text style={styles.metaText} numberOfLines={1} ellipsizeMode="tail">
            {selectedImage.meta}
          </Text>
        </View>
        {/* Arrows flank the card. D-pad left/right moves focus between them. */}
        <View style={styles.carouselRow}>
          <Button
            displayName="Previous"
            isNext={false}
            onTapped={handleTapped}
          />
          <ImageCard
            style={styles.imageContainer}
            imageUrl={selectedImage.url}
            description={selectedImage.description}
            slideDirection={slideDirection}
            a11yLabel={cardLabel}
            onImageLoaded={reportContentLoaded}
            prefetchUrls={prefetchUrls}
            // Animation tuning is applied natively but set from JS so it can be
            // adjusted with Fast Refresh (see cardTransition in theme.ts).
            {...cardTransition}
          />
          <Button
            displayName="Next"
            isNext={true}
            autoFocus={true}
            onTapped={handleTapped}
          />
        </View>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  background: {
    flex: 1,
    backgroundColor: colors.background,
  },
  mainContent: {
    flex: 1,
    paddingVertical: SAFE_ZONE_INSET,
    paddingHorizontal: SAFE_ZONE_INSET,
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  backgroundGlowTop: {
    position: 'absolute',
    top: -120,
    right: -70,
    width: 420,
    height: 420,
    borderRadius: 210,
    backgroundColor: 'rgba(17, 88, 148, 0.16)',
  },
  backgroundGlowBottom: {
    position: 'absolute',
    bottom: -120,
    left: -100,
    width: 340,
    height: 340,
    borderRadius: 170,
    backgroundColor: 'rgba(6, 53, 88, 0.14)',
  },
  carouselRow: {
    flex: 1,
    width: '100%',
    marginTop: 24,
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  imageContainer: {
    width: '80%',
    height: '100%',
  },
  headerPanel: {
    width: '80%',
    alignItems: 'center',
  },
  headerText: {
    ...tvTypography.title,
    color: colors.textPrimary,
    letterSpacing: -0.4,
    textAlign: 'center',
  },
  eyebrow: {
    ...tvTypography.caption,
    color: colors.accent,
    letterSpacing: 0.5,
    marginBottom: 8,
  },
  metaText: {
    ...tvTypography.caption,
    color: colors.textSecondary,
    letterSpacing: 0.4,
    marginTop: 8,
    textAlign: 'center',
  },
});
