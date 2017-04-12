# Webcam Capture
Library aimed to provide a unified interface to video capture across different platforms.

The library is not complete, many things are not implemented and the ones that are implemented need to be improved. Work on the library has stalled due not to having time to work on it. Use with caution.

## Project Goals

### Functionality
Library should allow a library user to do the following:

- Get a list of available backends
- Specify which backend to use
- Use multiple backends at once
- Get a list of all connected camera devices, with their respected unique camera ids and user-friendly device names
- Specify which camera to use based on on the unique id from the mentioned list
- Use multiple cameras at once
- Capture a video stream (video mode)
- Capture a still photo (photo mode)*
- Get a list of all supported image formats of a camera (YUV, RGB, MJPEG, etc.)
- Get a list of all supported resolutions when using a certain image format for capturing
- Get a list of all supported FPS when using a certain image format and resolution for capturing
- Specify in which image format, resolution and FPS to capture
- Get captured image frames in YUV or RGB image formats, possibly through a conversion if the original format differs
- Get captured image frames in the original format
- Get notified when a camera is connected/disconnected to/from a system
- Change brightness, contrast, saturation and possibly other settings that change how a captured image looks

### Supported platforms
- Windows
  - Windows XP and newer -- DirectShow backend
  - Windows Vista (Windows 7?) and newer -- Media Foundation backend
- OS X
  - OS X 10.7 and newer -- AV Foundation backend
- Linux
  - V4L/V4L2 backend

## Build
See [INSTALL.md](INSTALL.md).

## Contribute
1. Find a thing to fix/implement in [Issues](https://github.com/abibas/webcamCaptureNewDesign/issues) or come up with your own idea, [create a discussion issue](https://github.com/abibas/webcamCaptureNewDesign/new) for it and get a feedback.
2. Make sure that no one is already working on it.
3. [Fork the repo](https://help.github.com/articles/fork-a-repo).
4. Create your feature branch (`git checkout -b my-new-feature`).
5. Commit your changes (`git commit -am 'Add some feature'`).
6. Push to the branch (`git push origin my-new-feature`).
7. [Create new Pull Request](https://help.github.com/articles/using-pull-requests).

## Contact
Leave nurupo an email on the address he uses in commits or ping `nurupo` on `#tox-dev` at Freenode IRC ([Freenode Webchat](https://webchat.freenode.net/?channels=#tox-dev), [KiwiIRC](https://kiwiirc.com/client/chat.freenode.net/?theme=basic#tox-dev)).

## About
This project started as Google Summer of Code 2015 project for [Project Tox organization](https://www.google-melange.com/gsoc/org2/google/gsoc2015/tox).

Note that we don't expect to reach all of our goals by the end of GSoC and that some of the goals are even out of scope for the GSoC project.

## License
This project is licensed under [Apache 2.0](LICENSE).

This project uses [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) licensed code from [roxlu's Video Capture library](https://github.com/roxlu/video_capture).
