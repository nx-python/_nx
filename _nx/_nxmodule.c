#include <switch.h>

#include "Python.h"

PyDoc_STRVAR(module_doc,
"The _nx module is a thin wrapper around libnx functions\n");


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


static PyMethodDef module_methods[] = {
    {"hid_scan_input", _nx_hid_scan_input, METH_NOARGS, _nx_hid_scan_input_doc},
    {"hid_keys_down", _nx_hid_keys_down, METH_VARARGS, _nx_hid_keys_down_doc},
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
