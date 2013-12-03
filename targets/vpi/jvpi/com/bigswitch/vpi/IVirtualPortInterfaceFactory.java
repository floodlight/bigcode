package com.bigswitch.vpi;

import java.util.List; 
import java.util.Collection;
import java.util.Map; 
import java.net.InetAddress;

public interface IVirtualPortInterfaceFactory {

    IVirtualPortInterface Create(String createSpec); 

}