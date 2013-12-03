package com.bigswitch.vpi;

import java.util.List; 
import java.util.Collection;
import java.util.Map; 
import java.net.InetAddress;

public interface IVirtualPortInterface {

    public String getName() throws Exception; 
    public int setName(String name) throws Exception; 
    public String getType(); 
    public String getCreateSpec(); 

    public int DescriptorGet(); 
    
    public int Send(byte[] packet, int len); 
    public int Recv(byte[] packet, int block); 

    public int SendListenerCount(); 
    public int RecvListenerCount();
    public int SendListenersDrop();
    public int RecvListenersDrop();

    public void Close(); 

}