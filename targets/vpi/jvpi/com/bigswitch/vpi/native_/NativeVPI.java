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