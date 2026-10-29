// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

import {FocusManager} from '@amazon-devices/react-native-kepler';
import React, {useEffect, useRef} from 'react';
import {findNodeHandle, StyleSheet, View, ViewProps} from 'react-native';
import {colors} from '../theme';
import ButtonContainerNative from './ButtonContainerSpec';

/**
 * Focusable arrow button. The native ButtonContainer view owns focus handling
 * and the focus visuals (border and fill colour); this wrapper only supplies
 * the chevron artwork and maps native events to the onTapped callback.
 */
export interface ButtonContainerProps extends ViewProps {
  displayName?: string;
  isNext: boolean;
  /** Take focus when first shown, so remote navigation and VoiceView start here. */
  autoFocus?: boolean;
  onTapped?: (next: boolean) => void;
}

export const Button: React.FunctionComponent<ButtonContainerProps> = (
  props: ButtonContainerProps,
) => {
  const styles = getStyles();
  const {displayName, isNext, autoFocus, onTapped} = props;
  const lastActivationTimeRef = useRef(0);
  const nativeRef =
    useRef<React.ElementRef<typeof ButtonContainerNative>>(null);

  // Nothing is focused at launch, so the first D-pad press would only pick a
  // starting point. Focus is requested on the next tick because the native view
  // may not exist yet when the effect first runs.
  useEffect(() => {
    if (!autoFocus) {
      return;
    }
    const timer = setTimeout(() => {
      const tag = findNodeHandle(nativeRef.current);
      if (tag != null) {
        FocusManager.focus(tag);
      }
    }, 0);
    return () => clearTimeout(timer);
  }, [autoFocus]);

  const activateButton = () => {
    const now = Date.now();

    // Avoid double-advancing if both the built-in select event and the
    // custom native event are delivered for the same press.
    if (now - lastActivationTimeRef.current < 250) {
      return;
    }

    lastActivationTimeRef.current = now;
    if (onTapped) {
      onTapped(isNext);
    }
  };

  const handleTapped = () => {
    activateButton();
  };

  const handleSelect = () => {
    activateButton();
  };

  return (
    <View style={styles.wrapper}>
      <ButtonContainerNative
        ref={nativeRef}
        a11yLabel={displayName}
        onSelect={handleSelect}
        onTapped={handleTapped}
        accessible={true}
        accessibilityLabel={displayName}
        testID={isNext ? 'button-next' : 'button-last'}>
        <View style={styles.surface}>
          {/* Chevron: two borders of a square, rotated 45deg so no icon font
              or image asset is needed. */}
          <View
            style={[
              styles.chevron,
              isNext ? styles.chevronNext : styles.chevronPrev,
            ]}
          />
        </View>
      </ButtonContainerNative>
    </View>
  );
};

const getStyles = () =>
  StyleSheet.create({
    wrapper: {
      justifyContent: 'center',
      alignItems: 'center',
    },
    chevron: {
      width: 22,
      height: 22,
      borderColor: colors.textPrimary,
      borderBottomWidth: 4,
      borderLeftWidth: 4,
    },
    chevronPrev: {
      transform: [{rotate: '45deg'}],
      marginLeft: 8,
    },
    chevronNext: {
      transform: [{rotate: '-135deg'}],
      marginRight: 8,
    },
    surface: {
      width: 64,
      height: 104,
      justifyContent: 'center',
      alignItems: 'center',
    },
  });
