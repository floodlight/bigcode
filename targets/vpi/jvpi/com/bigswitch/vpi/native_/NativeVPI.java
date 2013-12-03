package com.bigswitch.vpi.native_;

import com.bigswitch.vpi.IVirtualPortInterface;
import com.bigswitch.vpi.native_.swig.VPI;

import java.util.List; 
import java.util.Collection;
import java.util.Map; 
import java.net.InetAddress;

public class NativeVPI implements IVirtualPortInterface {

    long vpiHandle; 

    public NativeVPI(String createSpec) {
        vpiHandle = VPI.vpi_create(createSpec); 
    }

    public String getName() {
        return VPI.vpi_name_get(vpiHandle); 
    }

    public int setName(String name) {
        return VPI.vpi_name_set(vpiHandle, name); 
    }

    public String getType() {
        return VPI.vpi_type_get(vpiHandle); 
    }

    public String getCreateSpec() {
        return VPI.vpi_get_create_spec(vpiHandle); 
    }

    public int DescriptorGet() {
        return VPI.vpi_descriptor_get(vpiHandle); 
    }
    
    public int Send(byte[] packet, int len) {
        return VPI.vpi_send(vpiHandle, packet, len); 
    }

    public int Recv(byte[] packet, int block) {
        return VPI.vpi_recv(vpiHandle, packet, packet.length, block); 
    }
    
    public int SendListenerCount()
    {
        return VPI.vpi_send_listener_count(vpiHandle); 
    }
    
    public int SendListenersDrop()
    {
        return VPI.vpi_send_listeners_drop(vpiHandle);
    }

    public int RecvListenerCount()
    {
        return VPI.vpi_recv_listener_count(vpiHandle); 
    }

    public int RecvListenersDrop()
    {
        return VPI.vpi_recv_listeners_drop(vpiHandle);
    }

    public void Close() {
        VPI.vpi_destroy(vpiHandle); 
        vpiHandle = 0; 
    }

}