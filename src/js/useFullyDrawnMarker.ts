// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

import {useReportFullyDrawn} from '@amazon-devices/kepler-performance-api';
import {useKeplerAppStateManager} from '@amazon-devices/react-native-kepler';
import {useCallback, useEffect, useRef} from 'react';

/**
 * Reports the "fully drawn" launch marker that the perf KPI tool uses to
 * compute time-to-fully-drawn (TTFD). Without it that KPI comes back empty.
 *
 * - Cool start: call the returned function once the first real content is on
 *   screen. Only the first call reports.
 * - Warm start: reported automatically when the app returns to the foreground,
 *   since its content is already drawn by then.
 */
export const useFullyDrawnMarker = () => {
  const reportFullyDrawn = useReportFullyDrawn();
  const appStateManager = useKeplerAppStateManager();
  const hasReportedColdStart = useRef(false);
  const previousState = useRef<string>(appStateManager.getCurrentState());

  useEffect(() => {
    const subscription = appStateManager.addAppStateListener(
      'change',
      (event: unknown) => {
        // The event is also used for display and reconfigure changes; only the
        // plain app-state strings matter here.
        if (typeof event !== 'string') {
          return;
        }
        if (
          /^(inactive|background)$/.test(previousState.current) &&
          event === 'active'
        ) {
          reportFullyDrawn();
        }
        previousState.current = event;
      },
    );
    return () => subscription.remove();
  }, [appStateManager, reportFullyDrawn]);

  return useCallback(() => {
    if (!hasReportedColdStart.current) {
      hasReportedColdStart.current = true;
      reportFullyDrawn();
    }
  }, [reportFullyDrawn]);
};
