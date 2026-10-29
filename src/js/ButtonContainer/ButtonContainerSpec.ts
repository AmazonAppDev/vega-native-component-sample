// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

// JS side of the native ButtonContainer: registers the view config so React
// Native knows which props and events the C++ view manager handles.
// @ts-ignore
import register from '@amazon-devices/react-native-kepler/Libraries/Utilities/registerGeneratedViewConfig';
import {ViewProps} from 'react-native';
import {BubblingEventHandler, HostComponent} from 'react-native-tscodegen-types';

type ChangeEvent = {};

interface NativeProps extends ViewProps {
  a11yLabel?: string;
  onSelect?: BubblingEventHandler<ChangeEvent>;
  onTapped?: BubblingEventHandler<ChangeEvent>;
}

const ButtonContainerComponent = 'ButtonContainer';

// The native side emits a "tapped" event. Depending on the runtime path it can
// arrive under several names, so each is mapped to the onTapped prop.
const viewConfig = {
  validAttributes: {
    a11yLabel: true,
  },
  bubblingEventTypes: {
    topTapped: {
      phasedRegistrationNames: {
        bubbled: 'onTapped',
        captured: 'onTappedCapture',
      },
    },
    tapped: {
      phasedRegistrationNames: {
        bubbled: 'onTapped',
        captured: 'onTappedCapture',
      },
    },
    onTapped: {
      phasedRegistrationNames: {
        bubbled: 'onTapped',
        captured: 'onTappedCapture',
      },
    },
    topOnTapped: {
      phasedRegistrationNames: {
        bubbled: 'onTapped',
        captured: 'onTappedCapture',
      },
    },
  },
};

register(ButtonContainerComponent, {
  uiViewClassName: 'ButtonContainer',
  ...viewConfig,
});

export default ButtonContainerComponent as unknown as HostComponent<NativeProps>;
