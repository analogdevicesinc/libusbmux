# libusbmux

libusbmux is a USB MUX (Multiplexer) library used to funnel USB communication from [OpenOCD](https://openocd.org) and Analog Devices' CrossCore(R) Debugger through one USB connection.

## Building libusbmux

libusbmux is only supported under Windows.

### Building for Windows using CMake

```sh
$ cd build
$ cmake .. -G "Visual Studio 14 2015" (for VS2015)
$ cmake --build . --config Release
```

#### Dependencies

- cmake
- C compiler (i.e. gcc or Visual Studio)

For more information on CMake and it's usage, see https://cmake.org/.

### Building for Windows using Autotools

```sh
$ ./bootstrap.sh
$ ./configure
$ make
```

#### Dependencies

- make
- libtool
- autoconf
- automake

For more information on Autotools and the GNU Build System, see https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html.

## Usage

Open a handle to the usbmux interface:

```c++
HANDLE hPipe;
DWORD ret = usbmux_open(&hPipe, timeout);
```

Acquire the lock to the usbmux before attempting any USB actions (blocking):
```c++
USB_MUX_ERROR err = usbmux_lock(hPipe);
```

Read from the connected USB device:
```c++
USB_MUX_ERROR err = usbmux_read(hPipe, pBuffer, numBytes, endpoint, timeout);
```

Write to the connected USB device:
```c++
USB_MUX_ERROR err = usbmux_write(hPipe, pBuffer, numBytes, endpoint, timeout);
```

Release the lock to the usbmux to allow other processes to use it:
```c++
USB_MUX_ERROR err = usbmux_unlock(hPipe);
```

Close the handle to the usbmux interface:
```c++
usbmux_close(hPipe);
```

## License

See LICENSE.md.
