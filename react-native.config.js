// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

module.exports = {
  dependency: {
    platforms: {
      kepler: {
        autolink: {
          SampleNativeUIComponents: {
            uiComponentName: '/com.amazondeveloper.nativecomponentexample',
            linkDynamic: true,
            provider: 'application',
            components: ['ImageCard', 'ButtonContainer'],
            turbomodules: [],
          },
        },
      },
    },
  },
};
