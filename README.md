## Build instructions for developers

The following instructions are given for a Linux environment. If you are working under Windows, you can use WSL as development environment.

### Initial setup

#### Environment

You can use the docker image witekio/mcu-toolchain-gnu-arm-embedded:24.01 as build environment for the firmware.

Otherwise, the following tools are required:

- CMake (tested with 3.26)
- arm-none-eabi toolchain v12.2.1
- Python 3

To flash and debug the project, you will need:

- J-Link Software


Pull the git submodules
```console
git submodule update --init --recursive
```

Install the python modules required to build the project:

```console
cd pc-nrfutil/requirements.txt
pip install -r requirements.txt
```

```console
cd pc-nrfutil/nordicsemi/dfu/azure_keyvault_signer/requirements.txt
pip install -r requirements.txt
```

### Build steps

The project is built using CMake and make. The build scripts will automatically configure the CMake and run the build instructions for bootloader and application.

#### Complete build

You can also perform a complete build using the following script:
```console
./buildall.sh
```

Build images are located in `build_release` folder.

### Deploy

To deploy on the target, connect a J-Link to the STM32 and power the target (USB or battery). Run the following command:

```console
cd build_release/Binary/manufacturing
./flash.sh flash
```

This will flash the bootloader and the user application on the target.

### Debug

In VSCode, using the [Cortex Debug Plugin](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug):

 1. Click on the `Run and Debug` button on the right of the window
 2. Click on the `Attach to Application` button on the top left of the window
 3. If RTT traces are activated, `RTT` output will appear in the `TERMINAL` tab of vscode.
 4. You can debug the target.