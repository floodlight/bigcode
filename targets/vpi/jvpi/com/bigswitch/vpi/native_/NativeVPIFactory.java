package com.bigswitch.vpi.native_;

import com.bigswitch.vpi.IVirtualPortInterfaceFactory;
import com.bigswitch.vpi.IVirtualPortInterface;
import com.bigswitch.vpi.native_.swig.VPI;

import java.util.List;
import java.util.Collection;
import java.util.Map;
import java.net.InetAddress;

public class NativeVPIFactory implements IVirtualPortInterfaceFactory  {

    public NativeVPIFactory() {
        System.loadLibrary("jvpi");
        VPI.vpi_init();
    }

    public IVirtualPortInterface Create(String createSpec) {
        NativeVPI nativeVpi = new NativeVPI(createSpec);
        return (IVirtualPortInterface) nativeVpi;
    }

}