// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

import {StyleSheet} from 'react-native';

// Tokens from the rn-tv-ui-best-practices guide (typography-and-color.md).
export const colors = {
  background: '#1A1A1A',
  textPrimary: '#E5E5E7',
  textSecondary: '#A1A1AA',
  accent: '#007AFF',
  focusBorder: '#00D4FF',
};

export const tvTypography = StyleSheet.create({
  caption: {fontSize: 20, lineHeight: 26, fontWeight: '400'},
  body: {fontSize: 24, lineHeight: 32, fontWeight: '400'},
  button: {fontSize: 22, lineHeight: 28, fontWeight: '600'},
  heading: {fontSize: 32, lineHeight: 40, fontWeight: '600'},
  title: {fontSize: 48, lineHeight: 56, fontWeight: '700'},
});

// Keep interactive content inside the 5% overscan-safe zone.
export const SAFE_ZONE_INSET = '5%';

// Card transition tuning; applied natively by ImageCard.
export const cardTransition = {
  transitionDuration: 700,
  slideDistance: 200,
  transitionEasing: 'easeInOutCubic',
};
