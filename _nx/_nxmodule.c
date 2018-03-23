#include <switch.h>

#include "Python.h"

/* Throw a Python error on bad IPC return code */
#define NX_ASSERT_IPC_OK(rc) { if (R_FAILED(rc)) { \
    PyErr_SetString(PyExc_OSError, "IPC call failed"); \
    return NULL; }}

PyDoc_STRVAR(module_doc,
"The _nx module is a thin wrapper around libnx functions\n");

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
    unsigned long long user_id;
    bool account_selected;

    rc = accountGetActiveUser((u128*) &user_id, &account_selected);
    NX_ASSERT_IPC_OK(rc);

    if (!account_selected)
        Py_RETURN_NONE;

    return PyLong_FromUnsignedLongLong(user_id);
}

/* end account */

static PyMethodDef module_methods[] = {
    {"hid_scan_input", _nx_hid_scan_input, METH_NOARGS, _nx_hid_scan_input_doc},
    {"hid_keys_down", _nx_hid_keys_down, METH_VARARGS, _nx_hid_keys_down_doc},
    {"account_initialize", _nx_account_initialize, METH_NOARGS, _nx_account_initialize_doc},
    {"account_get_active_user", _nx_account_get_active_user, METH_NOARGS, _nx_account_get_active_user_doc},
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
