# RTEMS Xilinx Runtime (XRT)
This repo contains RTEMS Support for Xilinx's Runtime (XRT).

XRT is a user and driver package from Xilinx that forms part of it's OpenCL framework. 
This repo ports XRT to RTEMS and provides drivers to load PDI and XCLBIN files to
support AIE integrations and workflows. The RTEMS `zocl` provides the support
needed to run AIE jobs and to interface to the XRT software

The code here is under heavy development and many pieces are yet to be 
completed. Please refer to the Status section for details.

-----------
## Equipment
You need to have a working Versal BSP for RTEMS. Please refer to the [Quick Start
Guide](https://docs.rtems.org/branches/master/user/start/index.html) for
details on doing this.

You will also need to build and install LibBSD for the Versal BSP.

The boards need to be running a suitable Trusted Firmware (TF-A) for the Cortex-A72.
The `BOOT.BIN` needs to be generated by Vivado and Vitis and the boot PDI is
suitable for loading a partial or DFX PDI. This is a complex process and beyond
the scope of what can be provided here.

-----------
## Building
1. Clone the RTEMS Xilinx XRT repo to your build host
```
git clone https://github.com/kiwichris/rtems-xilinx-xrt.git
```
2. We build in the source tree:
```
cd rtems-xilinx-xrt
```
3. Initialise the submodules:
```
git submodule init
```
4. Update the submodules:
```
git submodule update
```
5. Configure the build for the Versal BSP:
```
./waf distclean configure --prefix=/opt/rtems/6 --rtems-bsp=aarch64/xilinx_versal_lp64_vck190
```
6. Build:
```
./waf
```
7. Install in the configure prefix path:
```
./waf install
```
-----------
## Testing
The build includes a test executable called `xbutil`. The executable will initialse the
network interface and leave you at the command prompt. The command XTR `xutil2` is
also available.

The Platform Manager (PM) command provide a simple way to load a PDI. To load enter
at the RTEMS shell prompt:
```
pm acap load /net/xilinx/vck-190/dfx/1/pr0-rm1-bram.pdi
```

-----------
### PDI For Loading
The Versal cannot load from an LPD or FPD scaler processor a boot PDI. The PLM is
hard coded to only load partial PDI images. A partial PDI is created using a DFX
flow in Vivado. This is sometimes called the **Classic SoC Boot Flow**. This
name is confusing because the flow to create a suitable DFX PDI is not classic
and is involved.

-----------
## Status
### Todo
- XCLBIN loading
- AIE memory management
- AIE CU workers

### July 2022
- Platform Manager (PM) can load DFX PDI files
- RTEMS `zocl` driver framework in place
- XRT is builds
- Required Boost modules build
