# Vega Native UI Component Sample

This sample shows how to build a **native UI component** for Vega OS and use it from a React Native app. It is the Vega counterpart of the [native components](https://reactnative.dev/docs/intro-react-native-components) idea in React Native: most of the screen is ordinary React, and a few views are implemented in C++ against the Vega UI toolkit and rendered inside the React tree.

## What this sample demonstrates

- **Creating native views in C++** with the Vega UI toolkit and exposing them to React Native as JSX components (`<ImageCard>` and `<ButtonContainer>`).
- **Passing data from JS to native** through props, including strings, integers and arrays.
- **Sending events from native to JS**. The native button reports remote-control activation back to a React callback.
- **Hosting React children inside a native view**. The native button draws its own border and fill and mounts a React-rendered chevron as its child.
- **Native focus handling**. The native button is focusable with the remote's D-pad and draws its own focus state.
- **Native animation**. The card runs a slide and fade transition with the UI toolkit animator, driven by props set from JS.
- **Native image loading and prefetching** through the UI toolkit image loader and its cache.
- **Accessibility for native views**. Native views are invisible to screen readers unless they publish their own labels and roles. Both views do, so VoiceView announces the arrows and speaks each new card as it appears.
- **Launch performance marker**. The app reports when it is "fully drawn" (the first poster has loaded), so the Vega KPI tool can measure time to fully drawn.
- **TV-friendly UI**. Type sizes, colours, safe-zone margins and focus cues follow the [rn-tv-ui-best-practices](https://github.com/AmazonAppDev/devices-agent-skills/tree/main/skills/rn-tv-ui-best-practices) guide.

The app itself is a small carousel. A title and subtitle sit above a card that shows a poster image and description, with a focusable arrow on each side to step through six items. The content is a subset of the sample catalog at <https://giolaq.github.io/scrap-tv-feed/catalog.json>. It is embedded in the source (`src/js/catalog.ts`), and only the images are downloaded at runtime.

## Architecture: what is native and what is React Native

The screen is written in React Native (TypeScript). Two of its building blocks, the image card and the arrow buttons, are native views written in C++. The React Native side describes *what* to show and reacts to events. The native side does the drawing, focus handling, image loading and animation.

```text
┌─ React Native (TypeScript) ────────────────────────────────────────────────┐
│ ┌─ App ──────────────────────────────────────────────────────────────────┐ │
│ │ Screen layout, title and subtitle                                      │ │
│ │ Carousel state: current item and direction                             │ │
│ │ Passes props to the card, handles button taps                          │ │
│ └────────────────────────────────────────────────────────────────────────┘ │
│                                                                            │
│ ┌─ Button (x2: previous, next) ────┐  ┌─ ImageCard wrapper ──────────────┐ │
│ │ Draws the arrow chevron          │  │ Typed props for the native card  │ │
│ │ Maps the native 'tapped' event   │  │ (image, text, prefetch list,     │ │
│ │ to the onTapped callback         │  │ slide direction, tuning)         │ │
│ └──────────────────────────────────┘  └──────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────────────┘
          ▲ tapped    ▼ layout                        ▼ props                 
┌─ Native (C++) ─────────────────────────────────────────────────────────────┐
│ ┌─ ViewManagerFactory ───────────────────────────────────────────────────┐ │
│ │ Creates the native view manager for each component name                │ │
│ └────────────────────────────────────────────────────────────────────────┘ │
│                                                                            │
│ ┌─ ButtonContainer ────────────────┐  ┌─ ImageCard ──────────────────────┐ │
│ │ Focusable view with its own      │  │ Downloads, caches and prefetches │ │
│ │ border, fill and focus state     │  │ images (native image loader)     │ │
│ │ Handles OK / Select key presses  │  │ Lays out image + description     │ │
│ │ Hosts the React chevron inside   │  │ Slide / fade transition using    │ │
│ │ Emits 'tapped' back to React     │  │ the native animator              │ │
│ └──────────────────────────────────┘  └──────────────────────────────────┘ │
└────────────────────────────────────────────────────────────────────────────┘
```

### ImageCard

- **Native (C++)**, in [src/kepler/ImageCard](src/kepler/ImageCard): downloads, caches and prefetches the poster images with the native image loader, lays out the image and description text inside a bordered card, and runs the slide and fade transition with the native animator. It is published to screen readers as a live region, so a new card is spoken when it changes, and it tells JS when an image has loaded.
- **React Native (TypeScript)**, in [src/js/ImageCard](src/js/ImageCard): a typed component that passes the image URL, description, accessibility label, prefetch list, slide direction and animation tuning to the native card as props.

### ButtonContainer (the arrow buttons)

- **Native (C++)**, in [src/kepler/ButtonContainer](src/kepler/ButtonContainer): a focusable view that draws its own border, fill and focus state, listens for the remote's OK/Select key, emits a `tapped` event, and publishes its label and button role to screen readers. React children are mounted inside it.
- **React Native (TypeScript)**, in [src/js/ButtonContainer](src/js/ButtonContainer): draws the chevron that sits inside the native button and turns the native `tapped` event into an `onTapped` callback.

### The screen

- **React Native (TypeScript)**, in [src/js/App.tsx](src/js/App.tsx): lays out the title, the card and the two buttons, keeps track of the current item and direction, and holds the sample content ([catalog.ts](src/js/catalog.ts)) and the look and animation settings ([theme.ts](src/js/theme.ts)).

A small amount of glue connects the two sides: `ViewManagerFactory` creates the right native view for each component name, and the `*Spec.ts` files register those names and their props and events with React Native. See [Extending the sample](#extending-the-sample) for how to add your own.

## Getting started

### Prerequisites

- **Vega SDK 2.0 or later.** This sample requires SDK 2.0 and has been built with Vega SDK 2.0.x. Install the [Vega Developer Tools](https://developer.amazon.com/docs/vega/latest/install-vega-sdk.html), which include the `vega` CLI and the native toolchain.
- [Node.js](https://nodejs.org/) 22.14 or later (see `engines` in `package.json`).
- A Vega OS Fire TV Stick connected to your computer, or the Vega Virtual Device.
- *(Optional)* The Vega Studio extension for Visual Studio Code, if you prefer an IDE to the command line. See [Set up the Vega Studio extension](https://developer.amazon.com/docs/vega/latest/setup-extension).

### Command line or IDE

You can build and run this sample from the command line with the `vega` CLI, or from Visual Studio Code with the Vega Studio extension. The steps below use the command line. To use the IDE instead, install the extension (see the link above), open this folder in Visual Studio Code, and follow the Vega Studio documentation to build the app, start the Vega Virtual Device or select your Fire TV Stick, and run the app. Run `npm install` (step 1) first in either case.

### 1. Install dependencies

```sh
npm install
```

### 2. Build

A debug build loads its JavaScript from a dev server, which enables hot reload. A release build bundles the JavaScript into the package.

```sh
npm run build:debug      # debug, for development with hot reload
npm run build:release    # release, for performance checks (same as `npm run build`)
```

The build writes a package for each architecture under `build/`:

- `build/armv7-<variant>/nativecomponentexample_armv7.vpkg` runs on the Vega OS Fire TV Stick.
- `build/x86_64-<variant>/nativecomponentexample_x86_64.vpkg` runs on the Vega Virtual Device on x86_64 computers.
- `build/aarch64-<variant>/nativecomponentexample_aarch64.vpkg` runs on the Vega Virtual Device on Apple silicon.

`<variant>` is `debug` or `release`.

### 3. Run

**On a Fire TV Stick**

```sh
vega device list
vega run-app build/armv7-release/nativecomponentexample_armv7.vpkg
```

**On the Vega Virtual Device**

```sh
vega virtual-device start
vega run-app build/aarch64-release/nativecomponentexample_aarch64.vpkg   # Apple silicon
vega run-app build/x86_64-release/nativecomponentexample_x86_64.vpkg     # x86_64
```

### 4. Develop with hot reload

JavaScript changes (layout, styles, props) can be applied without rebuilding or reinstalling.

1. Build and install the **debug** package as shown above.
2. Start the dev server in a separate terminal:

   ```sh
   npm start
   ```

3. On a physical device, forward the dev server port so the device can reach your computer. Repeat this after the device sleeps or reboots:

   ```sh
   vega device start-port-forwarding --port 8081 --forward false
   ```

4. Launch the app. Saved edits to files under `src/js` are applied with Fast Refresh.

Changes to the C++ code under `src/kepler` always need a rebuild and a reinstall (`vega run-app`).

Animation tuning (`transitionDuration`, `slideDistance` and `transitionEasing`) lives in `cardTransition` in `src/js/theme.ts` and is read by the native card as props, so you can tune the transition with hot reload.

### Troubleshooting

- **Native changes do not show up on the device.** The build can leave a stale copy of the native library in `build/private/vega`, and the packager can then ship the old copy. Delete that folder (`rm -rf build/private/vega`) and build again. To check what was packaged, unpack the `.vpkg` (`zstd -dc <file>.vpkg | tar -x`) and inspect `lib/arm/*.so`.
- **The app shows a black screen in a debug build.** Check that `npm start` is running and that port forwarding is active, and look at the terminal running Metro for a JavaScript error.
- **The app logs.** Stream device logs with `vega device start-log-stream`. The native components log under the `ImageCardViewManager` and `buttoncontainer` loggers.

## Accessibility

Native views only appear to screen readers if they publish accessibility information themselves, so this sample does it in C++ (see [AccessibilityDelegate.h](src/kepler/AccessibilityDelegate.h)):

- **Labels and roles.** Each arrow button announces its label ("Previous" or "Next") and the button role. The label comes from the `a11yLabel` prop, which JS sets from the button's `displayName`.
- **Speaking the card.** The card is a polite live region. `App.tsx` builds a short label from the title, genre and year (the description stays on screen only, to keep the announcement brief) and passes it as `a11yLabel`. When it changes, VoiceView reads the new card without the card needing focus.
- **Screen title.** The title text has `role="heading"` so VoiceView can present it when the screen appears.
- **Initial focus.** The Next arrow takes focus at launch (`autoFocus`), so the first thing a screen-reader user hears is the Next button.

To try it, turn on VoiceView on a physical Fire TV device: Settings > Accessibility > VoiceView, or hold Back and Menu for a few seconds. VoiceView is not available on the Vega Virtual Device, and the setting cannot be changed from the command line on production firmware. You can also check what the app publishes without listening: the labels and roles appear as `description` and `role` attributes in the Appium page source (see [Appium](https://developer.amazon.com/docs/vega/latest/appium-install.html)).

To make your own native view accessible, install an accessibility delegate with `IAccessibilityService::setDelegate`, set the label and role in its `build` method, and call `invalidate` on the service whenever the label changes. Use one delegate per view, because a single view manager creates every view of its type.

## Native component configuration

A native component requires several pieces of configuration, and the names used in them must be consistent. This section lists them using the values from this sample.

### The names that must match

| Name | Where it is set |
| --- | --- |
| Component path `/com.amazondeveloper.nativecomponentexample` | `uiComponentName` in `react-native.config.js`, `kepler_install_components` in `CMakeLists.txt`, and `APMF_COMPONENT` in `ViewManagerFactory.cpp` |
| View name (`ImageCard`, `ButtonContainer`) | `components` in `react-native.config.js`, `uiViewClassName` in each `*Spec.ts`, and the `name ==` check in `ViewManagerFactory::makeViewManager` |

If the component path differs between the three places, or a view name differs between the three places, the view is not found at runtime.

### Autolinking: `react-native.config.js`

Tells the build which native components this package provides:

```js
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
```

### Native build: `CMakeLists.txt`

```cmake
find_package(Apmf REQUIRED)
find_package(Kepler REQUIRED)

# Generates the C++ headers for the UI toolkit interfaces the views use.
kepler_add_idl_import_library(
  ${IDL_NAME}
  PACKAGES
  com.amazon.kepler.uitoolkit.graphics
  com.amazon.kepler.uitoolkit.view
  com.amazon.kepler.uitoolkit.react
  com.amazon.kepler.input.events
)

set(SOURCES
    src/kepler/ViewManagerFactory.cpp
    src/kepler/ButtonContainer/ButtonContainerViewManager.cpp
    src/kepler/ImageCard/ImageCardViewManager.cpp
)

kepler_add_library(${LIB_NAME} SHARED ${SOURCES})
set_target_properties(${LIB_NAME} PROPERTIES
  PREFIX ""
  OUTPUT_NAME "com.amazondeveloper.nativecomponentexample"
)
target_link_libraries(${LIB_NAME} PRIVATE ${IDL_NAME})

# Makes the library available in-process under the component path.
kepler_install_components(${LIB_NAME} IN_PROCESS
  "/com.amazondeveloper.nativecomponentexample")
```

- List every `.cpp` file in `SOURCES`.
- List every UI toolkit IDL package your views use in `kepler_add_idl_import_library`. This sample also needs `com.amazon.kepler.input.events` for the OK/Select key handling.

### Registering the factory: `ViewManagerFactory.cpp`

`ViewManagerFactory` implements `IViewManagerFactory`. React Native calls `makeViewManager` with a view name, and the factory returns the matching view manager. `APMF_COMPONENT` publishes the factory under the component path:

```cpp
Ptr<IViewManager> ViewManagerFactory::makeViewManager(StringView name) {
    // ... get the view and graphics modules ...
    if (name == "ImageCard") {
        return Ptr<ImageCardViewManager>::Make(viewModule, graphicsModule);
    }
    if (name == "ButtonContainer") {
        return Ptr<ButtonContainerViewManager>::Make(viewModule, graphicsModule);
    }
    throw apmf::InvalidArgumentError("Cannot make UI component");
}

APMF_COMPONENT("/com.amazondeveloper.nativecomponentexample",
               nativeuicomponents::ViewManagerFactory);
```

### JS view config: `*Spec.ts`

Each component has a spec file that registers its view name, props, events and commands with React Native. The `uiViewClassName` is the name the factory receives:

```ts
register(ImageCardComponent, {
  uiViewClassName: 'ImageCard',
  ...viewConfig,
});
```

### View manager callbacks

A view manager always implements `IViewManager`. Add the delegates it needs:

| Interface | Use it to |
| --- | --- |
| `IPropsDelegate` | Declare props (`registerProps`) and receive their updates (`updateProps`) |
| `ILayoutDelegate` | Receive the layout React Native computed (`updateLayout`) and size the native view |
| `IContextDelegate` | Get the context, which gives access to the event emitter for sending events to JS |
| `IMountDelegate` | Add and remove React children when the view is a container (`mountChild`, `unmountChild`) |
| `ICommandDispatchDelegate` | Handle commands sent from JS (`dispatchCommand`) |

For each view, the factory calls `makeView` once per instance. Props arrive through `registerProps`, then `updateProps` and `updateLayout`. When a view has children, the children receive their callbacks first and the parent's `mountChild` follows.

## Extending the sample

### Change the behavior of an existing component

- **Add or change a prop on `ImageCard`.** Add the prop to the `NativeProps` interface and the `validAttributes` list in `ImageCardSpec.ts`, add it to `ImageCardProps` in `ImageCard.tsx`, and declare it in `ImageCardViewManager::registerProps` (for example with `stringPropWithDefault`, `intPropWithDefault` or `arrayProp`). Read it in `ImageCardViewManager::updateProps`. A prop must be declared on both the JS and the native side, or it never reaches your native code.
- **Change the look.** Colours, borders and radii are set in native code in each view manager's `makeView`, and in the focus listener for the button. Text, spacing and type sizes used by the JS views come from `src/js/theme.ts`.
- **Change the animation.** The transition is in `ImageCardViewManager::startTransitionAnimation`. It builds a `IAnimator` with a duration, an interpolator and start and end values, and applies each frame's value to the card's opacity and transformation matrix.
- **Change the content.** Edit `src/js/catalog.ts`.

### Add a new native component

The steps below use a component named `Badge` as an example.

1. **Write the view manager.** Create `src/kepler/Badge/BadgeViewManager.h` and `.cpp`. Model it on `ButtonContainerViewManager` (a container that hosts React children and emits events) or `ImageCardViewManager` (a view that reads props and manages its own content). A view manager implements `IViewManager` plus the delegates it needs, for example `IPropsDelegate` for props, `ILayoutDelegate` for layout, `IMountDelegate` for React children and `ICommandDispatchDelegate` for commands from JS.
2. **Add the source to the build.** List the new `.cpp` file in `SOURCES` in `CMakeLists.txt`, and add any new UI toolkit IDL packages it uses.
3. **Register it with the factory.** In `ViewManagerFactory::makeViewManager`, return your new view manager for the name `"Badge"`.
4. **Declare it for autolinking.** Add `'Badge'` to the `components` list in `react-native.config.js`.

   The component path and view name must match across the build, factory, autolink and JS files. See [Native component configuration](#native-component-configuration).
5. **Register the view config in JS.** Create `src/js/Badge/BadgeSpec.ts` following `ImageCardSpec.ts` (for props) or `ButtonContainerSpec.ts` (for events), and a `Badge.tsx` wrapper component.
6. **Rebuild and reinstall.** Native changes need `npm run build:debug` (or `build:release`) followed by `vega run-app`.

### Documentation

The native UI toolkit documentation is not yet public.

For React Native on Vega, see the [Vega documentation](https://developer.amazon.com/docs/vega/latest/) and the [React Native native components overview](https://reactnative.dev/docs/intro-react-native-components).

## Known issues

- **Native (C++) changes can silently not reach the device after an incremental rebuild.** The packaging step can ship a stale copy of the native library from the last clean build, so the device keeps running old native code even though the build succeeds. Delete `build/private/vega` before rebuilding native code (see [Troubleshooting](#troubleshooting)).

- **The `vega exec perf` tools need a real terminal on a physical device.** Run from a non-interactive shell (a script, CI job or an automated agent), the KPI tool fails on a physical Fire TV Stick with "Starting trace collecting service failed" and `perf record` fails with "Operation not supported by device", so no measurements are produced. Run in an interactive terminal, or wrapped in a pseudo-terminal, the same commands work on both the physical device and the Vega Virtual Device. Even then, on the physical device in our tests the UI fluidity, warm start and background memory KPIs finished their iterations but reported no values. Numbers from the Virtual Device are not representative of real hardware.

## Project layout

```
vega-native-component-sample/
├── index.js                       App entry point
├── app.json                       App name (must match the component id in manifest.toml)
├── manifest.toml                  Vega package manifest
├── package.json                   Scripts and dependencies
├── react-native.config.js         Declares the native components to autolink
├── CMakeLists.txt                 Native build
└── src/
    ├── js/                        React Native code
    │   ├── App.tsx
    │   ├── theme.ts
    │   ├── catalog.ts
    │   ├── useFullyDrawnMarker.ts  Reports the "fully drawn" launch marker
    │   ├── ImageCard/             JS wrapper and view config for the native card
    │   └── ButtonContainer/       JS wrapper and view config for the native button
    └── kepler/                    Native (C++) code
        ├── ViewManagerFactory.*
        ├── AccessibilityDelegate.h  Shared screen-reader label/role/live-region publisher
        ├── ImageCard/
        └── ButtonContainer/
```

## License

This sample is licensed under the MIT-0 License. See [LICENSE](LICENSE).
