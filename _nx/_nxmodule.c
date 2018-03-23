#include <switch.h>
#include <limits.h>

#include "Python.h"

/* Throw a Python error on bad IPC return code */
#define NX_ASSERT_IPC_OK(rc) { if (R_FAILED(rc)) { \
    PyErr_SetString(PyExc_OSError, "IPC call failed"); \
    return NULL; }}

PyDoc_STRVAR(module_doc,
"The _nx module is a thin wrapper around libnx functions\n");

/* internal utils */

static PyObject *
_PyLong_FromU128(u128 int_in)
{
    unsigned long high, low;
    PyObject *result, *result_low, *sixty_four;

    high = int_in >> 64;
    low = int_in;

    result = PyLong_FromUnsignedLong(high);
    result_low = PyLong_FromUnsignedLong(low);
    sixty_four = PyLong_FromUnsignedLong(64);
    result = PyNumber_InPlaceLshift(result, sixty_four);
    result = PyNumber_InPlaceOr(result, result_low);

    Py_DECREF(result_low);
    Py_DECREF(sixty_four);

    return result;
}

static int
_PyLongToU128Converter(PyObject *pylong_in, u128 *int_out)
{
    u128 result;
    PyObject *in_high, *sixty_four;

    // XXX: add type checking!!!

    sixty_four = PyLong_FromUnsignedLong(64);
    in_high = PyNumber_Rshift(pylong_in, sixty_four);

    result = PyLong_AsUnsignedLongMask(in_high);
    result <<= 64;
    result |= PyLong_AsUnsignedLongMask(pylong_in);

    Py_DECREF(in_high);
    Py_DECREF(sixty_four);

    *int_out = result;
    return 1;
}

/* end internal utils */

/* hid */

PyDoc_STRVAR(_nx_hid_scan_input_doc, ""); // TODO

static PyObject *
_nx_hid_scan_input(PyObject *self, PyObject *args)
{
    hidScanInput();
    Py_RETURN_NONE;
}


PyDoc_STRVAR(_nx_hid_keys_down_doc, ""); // TODO

static PyObject *
_nx_hid_keys_down(PyObject *self, PyObject *args)
{
    int controller_id;
    unsigned long result;

    if (!PyArg_ParseTuple(args, "i", &controller_id))
        return NULL;

    result = hidKeysDown(controller_id);

    return PyLong_FromUnsignedLong(result);
}

/* end hid */

/* account */

PyDoc_STRVAR(_nx_account_initialize_doc, ""); // TODO

static PyObject *
_nx_account_initialize(PyObject *self, PyObject *args)
{
    Result rc;

    rc = accountInitialize();
    NX_ASSERT_IPC_OK(rc);

    Py_RETURN_NONE;
}


PyDoc_STRVAR(_nx_account_get_active_user_doc, ""); // TODO

static PyObject *
_nx_account_get_active_user(PyObject *self, PyObject *args)
{
    Result rc;
    u128 user_id;
    bool account_selected;

    rc = accountGetActiveUser(&user_id, &account_selected);
    NX_ASSERT_IPC_OK(rc);

    if (!account_selected)
        Py_RETURN_NONE;

    return _PyLong_FromU128(user_id);
}

/* end account */

/* fs */

PyDoc_STRVAR(_nx_fs_mount_savedata_doc,
    "fs_mount_savedata(mountpoint, title_id, user_id)"); // TODO

static PyObject *
_nx_fs_mount_savedata(PyObject *self, PyObject *args)
{
    const char *mountpoint;
    unsigned long title_id;
    u128 user_id;
    FsFileSystem tmpfs;
    Result rc;
    int ret;

    if (!PyArg_ParseTuple(args, "skO&", &mountpoint, &title_id, _PyLongToU128Converter, &user_id))
        return NULL;

    rc = fsMount_SaveData(&tmpfs, title_id, user_id);
    NX_ASSERT_IPC_OK(rc);

    ret = fsdevMountDevice(mountpoint, tmpfs);
    if (ret == -1) {
        PyErr_SetString(PyExc_OSError, "fsdevMountDevice() Failed");
        return NULL;
    }

    Py_RETURN_NONE;
}

/* end fs */

static PyMethodDef module_methods[] = {
    {"hid_scan_input", _nx_hid_scan_input, METH_NOARGS, _nx_hid_scan_input_doc},
    {"hid_keys_down", _nx_hid_keys_down, METH_VARARGS, _nx_hid_keys_down_doc},
    {"account_initialize", _nx_account_initialize, METH_NOARGS, _nx_account_initialize_doc},
    {"account_get_active_user", _nx_account_get_active_user, METH_NOARGS, _nx_account_get_active_user_doc},
    {"fs_mount_savedata", _nx_fs_mount_savedata, METH_VARARGS, _nx_fs_mount_savedata_doc},
    {NULL, NULL}
};

struct PyModuleDef _PyNX_Module = {
    PyModuleDef_HEAD_INIT,
    "nx",
    module_doc,
    -1,
    module_methods,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC
PyInit__nx(void)
{
    PyObject *m = PyModule_Create(&_PyNX_Module);
    return m;
}
