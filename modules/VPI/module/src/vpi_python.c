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

#include <VPI/vpi_config.h>
#include <VPI/vpi.h>
#include "vpi_int.h"
#include "vpi_log.h"
#include <AIM/aim_pvs_file.h>

#if VPI_CONFIG_INCLUDE_PYTHON_BINDINGS == 1

#define VPI_LOG_PREFIX1 ".python"
#include "vpi_log.h"

#include <Python.h>

#define PY_EXCEPT_VPI_VALUE(_returnObject)                              \
    do {                                                                \
        PyErr_SetString(PyExc_ValueError, "vpi object required.");      \
        _returnObject = NULL;                                           \
    } while(0)



/**************************************************************************//**
 *
 * Hand-coded wrappers for different function
 * signatures. Should probably SWIG this stuff.
 *
 *
 *****************************************************************************/
#define GENERIC_WRAPPER(_returntype, _returnformat, _function, _sighandler) \
    static PyObject*                                                    \
    py__##_function##__(PyObject* self, PyObject* args)                 \
    {                                                                   \
        _returntype rv;                                                 \
        PyObject* prv;                                                  \
        prv = _sighandler(self, args, _function, &rv);                  \
        return prv;                                                     \
    }

#define RINT_WRAPPER(_function, _sighandler)            \
    GENERIC_WRAPPER(int, "%d", _function, _sighandler)
#define RSTR_WRAPPER(_function, _sighandler)                    \
    GENERIC_WRAPPER(const char*, "%s", _function, _sighandler)

static PyObject*
signature_int_vpi_string__(PyObject* self, PyObject* args,
                           int (*function)(vpi_t, const char*),
                           int* rv)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;
    char* s;

    if(PyArg_ParseTuple(args, "Os", &pvpi, &s) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        *rv = function(vpi, s);
        ro = Py_BuildValue("i", *rv);
    }
    return ro;
}

static PyObject*
signature_int_vpi_vpi__(PyObject* self, PyObject* args,
                        int (*function)(vpi_t, vpi_t),
                        int* rv)
{
    PyObject* ro;
    PyObject* pvpi;
    PyObject* pvpi2;
    vpi_t vpi;
    vpi_t vpi2;

    if(PyArg_ParseTuple(args, "OO", &pvpi, &pvpi2) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        vpi2 = (vpi_t)PyCObject_AsVoidPtr(pvpi2);
        *rv = function(vpi, vpi2);
        ro = Py_BuildValue("i", *rv);
    }
    return ro;
}

static PyObject*
signature_int_vpi__(PyObject* self, PyObject* args,
                    int (*function)(vpi_t),
                    int* rv)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;

    if(PyArg_ParseTuple(args, "O", &pvpi) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        *rv = function(vpi);
        ro = Py_BuildValue("i", *rv);
    }
    return ro;
}

static PyObject*
signature_string_vpi__(PyObject* self, PyObject* args,
                       const char* (*function)(vpi_t),
                       const char** rv)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;

    if(PyArg_ParseTuple(args, "O", &pvpi) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        *rv = function(vpi);
        ro = Py_BuildValue("s", *rv);
    }
    return ro;
}

static void
pyvpi_destroy__(void* ptr)
{
    vpi_destroy(ptr);
}

PyObject*
py__vpi_create(PyObject* self, PyObject* args)
{
    PyObject* ro = NULL;
    vpi_t nvpi;
    char* create_spec;

    VPI_FENTER("");

    if(PyArg_ParseTuple(args, "s", &create_spec)) {
        nvpi = vpi_create(create_spec);
        if(nvpi) {
            ro = PyCObject_FromVoidPtr(nvpi, pyvpi_destroy__);
        }
        else {
        }
    }
    VPI_FEXIT("obj=%p", ro);
    return ro;
}

RSTR_WRAPPER(vpi_get_create_spec, signature_string_vpi__);
RSTR_WRAPPER(vpi_type_get, signature_string_vpi__);
RSTR_WRAPPER(vpi_name_get, signature_string_vpi__);
RINT_WRAPPER(vpi_name_set, signature_int_vpi_string__);
RINT_WRAPPER(vpi_ref, signature_int_vpi__);
RINT_WRAPPER(vpi_unref, signature_int_vpi__);
RINT_WRAPPER(vpi_drain, signature_int_vpi__);
RINT_WRAPPER(vpi_descriptor_get, signature_int_vpi__);
RSTR_WRAPPER(vpi_get_recv_spec, signature_string_vpi__);
RSTR_WRAPPER(vpi_get_send_to_spec, signature_string_vpi__);

RINT_WRAPPER(vpi_add_recv_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_add_recv_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_remove_recv_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_remove_recv_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_recv_listener_count, signature_int_vpi__);
RINT_WRAPPER(vpi_recv_listeners_drop, signature_int_vpi__);

RINT_WRAPPER(vpi_add_send_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_add_send_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_remove_send_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_remove_send_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_send_listener_count, signature_int_vpi__);
RINT_WRAPPER(vpi_send_listeners_drop, signature_int_vpi__);

RINT_WRAPPER(vpi_add_sendrecv_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_add_sendrecv_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_remove_sendrecv_listener, signature_int_vpi_vpi__);
RINT_WRAPPER(vpi_remove_sendrecv_listener_spec, signature_int_vpi_string__);
RINT_WRAPPER(vpi_sendrecv_listener_count, signature_int_vpi__);
RINT_WRAPPER(vpi_sendrecv_listeners_drop, signature_int_vpi__);

static PyObject*
py__vpi_send(PyObject* self, PyObject* args)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;
    unsigned char* b;
    int size;
    int rv;

    VPI_FENTER("");

    if(PyArg_ParseTuple(args, "Os#", &pvpi, &b, &size) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        rv = vpi_send(vpi, b, size);
        ro =  Py_BuildValue("i", rv);
    }
    VPI_FEXIT("rv=%d", rv);
    return ro;
}

static PyObject*
py__vpi_ioctl(PyObject* self, PyObject* args)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;
    unsigned char* b;
    int size;
    int rv;
    int cmd;


    VPI_FENTER("");

    if(PyArg_ParseTuple(args, "Ois#", &pvpi, &cmd, &b, &size) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        rv = vpi_ioctl(vpi, cmd, b, size);
        ro =  Py_BuildValue("i", rv);
    }
    VPI_FEXIT("rv=%d", rv);
    return ro;
}

static PyObject*
py__vpi_recv(PyObject* self, PyObject* args)
{
    PyObject* ro;
    PyObject* pvpi;
    vpi_t vpi;
    int block;
    int rv;
    unsigned char buffer[VPI_CONFIG_MAX_PACKET];

    VPI_FENTER("");

    if(PyArg_ParseTuple(args, "Oi", &pvpi, &block) == 0) {
        PY_EXCEPT_VPI_VALUE(ro);
    }
    else {
        vpi = (vpi_t)PyCObject_AsVoidPtr(pvpi);
        rv = vpi_recv(vpi, buffer, sizeof(buffer), block);
        if(rv < 0) {
            PyErr_SetString(PyExc_IOError,
                            "vpi_recv() failure");
        }
        else if(rv == 0) {
            Py_INCREF(Py_None);
            ro = Py_None;
        }
        else {
            ro = Py_BuildValue("s#", buffer, rv);
        }
    }

    VPI_FEXIT("rv=%d", rv);
    return ro;
}



static PyObject*
py__vpi_config_show(PyObject* self, PyObject* args)
{
    vpi_config_show(&aim_pvs_stdout);
    Py_RETURN_NONE;
}


#if VPI_CONFIG_INCLUDE_BRIDGING == 1

static void
py__vpi_bridge_destroy(void* p)
{
    //vpi_bridge_t bridge = (vpi_bridge_t)p;
    //vpi_bridge_destroy(bridge);
}

static PyObject*
py__vpi_bridge_create(PyObject* self, PyObject* args)
{
    PyObject* pv1, *pv2;
    vpi_t v1, v2;
    vpi_bridge_t bridge;

    if(PyArg_ParseTuple(args, "OO", &pv1, &pv2) == 0) {
        return NULL;
    }
    v1 = (vpi_t)PyCObject_AsVoidPtr(pv1);
    v2 = (vpi_t)PyCObject_AsVoidPtr(pv2);

    printf("bridgeCreate: v1=%p, name=%s, v2=%p, name=%s\n", v1, vpi_name_get(v1),
           v2, vpi_name_get(v2));
    bridge = vpi_bridge_create(v1, v2);
    if(bridge == NULL) {
        PyErr_SetString(PyExc_IOError,
                        "vpi_bridge_create() failed.");
        return NULL;
    }
    else {
        Py_INCREF(pv1);
        Py_INCREF(pv2);
        return PyCObject_FromVoidPtr(bridge, py__vpi_bridge_destroy);
    }
}

static PyObject*
py__vpi_bridge_start(PyObject* self, PyObject* args)
{
    PyObject* pvb;
    vpi_bridge_t vb;
    int rv;

    if(PyArg_ParseTuple(args, "O", &pvb) == 0) {
        return NULL;
    }
    vb = (vpi_bridge_t)PyCObject_AsVoidPtr(pvb);
    rv = vpi_bridge_start(vb);
    return Py_BuildValue("i", rv);
}

static PyObject*
py__vpi_bridge_stop(PyObject* self, PyObject* args)
{
    PyObject* pvb;
    vpi_bridge_t vb;
    int rv;

    if(PyArg_ParseTuple(args, "O", &pvb) == 0) {
        return NULL;
    }
    vb = (vpi_bridge_t)PyCObject_AsVoidPtr(pvb);
    rv = vpi_bridge_stop(vb);
    return Py_BuildValue("i", rv);
}

#else

static PyObject*
py__vpi_bridge_create(PyObject* self, PyObject* args)
{
    PyErr_SetString(PyExc_RuntimeError,
                    "vpiBridgeSupport was not included in this build.");
    return NULL;
}

static PyObject*
py__vpi_bridge_start(PyObject* self, PyObject* args)
{
    PyErr_SetString(PyExc_RuntimeError,
                    "vpiBridgeSupport was not included in this build.");
    return NULL;
}

static PyObject*
py__vpi_bridge_stop(PyObject* self, PyObject* args)
{
    PyErr_SetString(PyExc_RuntimeError,
                    "vpiBridgeSupport was not included in this build.");
    return NULL;
}

#endif




PyMethodDef vpi_methods[] =
    {
#define METHOD(_name, _handler, _desc)                  \
        { _name, _handler, METH_VARARGS, _desc }

        METHOD("Create", py__vpi_create,
               "Create(create_spec) -> new VPI object."),
        METHOD("NameGet", py__vpi_name_get__,
               "NameGet() -> Return the name of the VPI object."),
        METHOD("NameSet", py__vpi_name_set__,
               "NameSet() -> Set the name of the VPI object."),
        METHOD("GetCreateSpec", py__vpi_get_create_spec__,
               "GetCreateSpec() -> return this VPI objects creation spec."),
        METHOD("TypeGet", py__vpi_type_get__,
               "TypeGet() -> Return the type of this VPI."),
        METHOD("Send", py__vpi_send, "Send() -> Send a packet."),
        METHOD("Ioctl", py__vpi_ioctl, "Ioctl() -> Send an Ioctl message."),
        METHOD("Recv", py__vpi_recv, "Recv(block) -> Recv a packet."),
        METHOD("Drain", py__vpi_drain__,
               "Drain() -> Drain all queued packets."),
        METHOD("Ref", py__vpi_ref__, "Increase a VPI's reference count."),
        METHOD("Unref", py__vpi_unref__,
               "Descrease a VPI's reference count."),
        METHOD("ConfigShow", py__vpi_config_show,
               "ConfigShow() -> Show compile time options for the VPI library."),
        METHOD("BridgeCreate", py__vpi_bridge_create,
               "BridgeCreate(v1, v2) -> Create VPI bridge between two VPI objects."),
        METHOD("BridgeStart", py__vpi_bridge_start,
               "BridgeStart(vb) -> Start the given bridge."),
        METHOD("BridgeStop", py__vpi_bridge_stop,
               "BridgeStop(vb) -> Stop the given bridge."),
        METHOD("DescriptorGet", py__vpi_descriptor_get__,
               "Get the underlying descriptor, if applicable."),
        METHOD("GetRecvSpec", py__vpi_get_recv_spec__,
               "Get the VPI receive specification."),
        METHOD("GetSendToSpec", py__vpi_get_send_to_spec__,
               "Get the SendTo specification."),

        METHOD("AddRecvListenerSpec", py__vpi_add_recv_listener_spec__,
               "Add the given VPI spec as a receive listener."),
        METHOD("AddRecvListener", py__vpi_add_recv_listener__,
               "Add the given VPI as a receive listener."),
        METHOD("RemoveRecvListenerSpec", py__vpi_remove_recv_listener_spec__,
               "Remote the given VPI spec as a receive listener."),
        METHOD("RemoveRecvListener", py__vpi_remove_recv_listener__,
               "Remove the given VPI as a receive listener."),
        METHOD("RecvListenerCount", py__vpi_recv_listener_count__,
               "Get the count of current receive listeners."),
        METHOD("RecvListenersDrop", py__vpi_recv_listeners_drop__,
               "Drop all receive listeners."),

        METHOD("AddSendListenerSpec", py__vpi_add_send_listener_spec__,
               "Add the given VPI spec as a send listener."),
        METHOD("AddSendListener", py__vpi_add_send_listener__,
               "Add the given VPI as a send listener."),
        METHOD("RemoveSendListenerSpec", py__vpi_remove_send_listener_spec__,
               "Remote the given VPI spec as a send listener."),
        METHOD("RemoveSendListener", py__vpi_remove_send_listener__,
               "Remove the given VPI as a send listener."),
        METHOD("SendListenerCount", py__vpi_send_listener_count__,
               "Get the count of current send listeners."),
        METHOD("SendListenersDrop", py__vpi_send_listeners_drop__,
               "Drop all send listeners."),

        METHOD("AddSendRecvListenerSpec", py__vpi_add_sendrecv_listener_spec__,
               "Add the given VPI spec as a sendrecv listener."),
        METHOD("AddSendRecvListener", py__vpi_add_sendrecv_listener__,
               "Add the given VPI as a sendrecv listener."),
        METHOD("RemoveSendRecvListenerSpec", py__vpi_remove_sendrecv_listener_spec__,
               "Remote the given VPI spec as a sendrecv listener."),
        METHOD("RemoveSendRecvListener", py__vpi_remove_sendrecv_listener__,
               "Remove the given VPI as a sendrecv listener."),
        METHOD("SendRecvListenerCount", py__vpi_sendrecv_listener_count__,
               "Get the count of current sendrecv listeners."),
        METHOD("SendRecvListenersDrop", py__vpi_sendrecv_listeners_drop__,
               "Drop all sendrecv listeners."),


        METHOD(NULL, NULL, NULL)
    };


void initpyvpi(void)
{

#if VPI_CONFIG_INCLUDE_PYTHON_DEBUG_LOGGING == 1
    char* fname = getenv("PYVPI_DEBUG_LOG");
    if(fname) {
        aim_pvs_t* pvs = aim_pvs_fopen(fname, "a");
        aim_log_pvs_set(AIM_LOG_STRUCT_POINTER, pvs);
    }
#endif

    vpi_init();
    Py_InitModule4("pyvpi",   // name of the module
                   vpi_methods,  // name of the method table
                   "C VPI class", // doc string for module
                   0,   // last two never change
                   PYTHON_API_VERSION);
    return;
}

#endif /* VPI_CONFIG_INCLUDE_PYTHON_BINDING */
