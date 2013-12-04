/****************************************************************
 *
 *        Copyright 2013, Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 ***************************************************************/

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