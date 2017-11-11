# soundtouch

A Particle library for basic control of Bose Soundtouch speakers.

## Usage

1. Have a Soundtouch speaker on your network

2. Have a Particle device on your network

3. Run a project using this library on your Particle device

4. Observe that this library discovers your Soundtouch speaker

5. Observe thsat this library doesn't do much useful to control it yet

6. Send a pull request implementing the thing you want to do

See the [examples](examples) folder for more details.

## Documentation

The major API is the Speaker class.  Currently the best way to obtain a Speaker class is to either use the Soundtouch class to discover one, or to instantiate directly with an IP address.

## Contributing

Here's how you can make changes to this library and eventually contribute those changes back.

To get started, [clone the library from GitHub to your local machine](https://help.github.com/articles/cloning-a-repository/).

Change the name of the library in `library.properties` to something different. You can add your name at then end.

Modify the sources in <src> and <examples> with the new behavior.

To compile an example, use `particle compile examples/usage` command in [Particle CLI](https://docs.particle.io/guide/tools-and-features/cli#update-your-device-remotely) or use our [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/#compiling-code).

After your changes are done you can upload them with `particle library upload` or `Upload` command in the IDE. This will create a private (only visible by you) library that you can use in other projects. Do `particle library add soundtouch_myname` to add the library to a project on your machine or add the soundtouch_myname library to a project on the Web IDE or Desktop IDE.

At this point, you can create a [GitHub pull request](https://help.github.com/articles/about-pull-requests/) with your changes to the original library. 

If you wish to make your library public, use `particle library publish` or `Publish` command.

## LICENSE
Copyright 2017 Matt Bornski

Licensed under the MIT license
