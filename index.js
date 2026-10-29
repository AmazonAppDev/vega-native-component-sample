// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: MIT-0

// App entry point: registers the root component under the name from app.json,
// which must match the interactive component id launched by manifest.toml.
import { AppRegistry, LogBox } from 'react-native';
import { App } from './src/js/App';
import { name as appName } from './app.json';

// Temporary workaround for problem with nested text
// not working currently.
LogBox.ignoreAllLogs();

AppRegistry.registerComponent(appName, () => App);
