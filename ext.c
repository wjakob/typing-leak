#include <Python.h>
#include <structmember.h>

/* Box holds a reference but has no tp_traverse, so a cycle through it is
   invisible to the garbage collector and is never collected. */
typedef struct { PyObject_HEAD PyObject *ref; } Box;
static void Box_dealloc(Box *b) { Py_XDECREF(b->ref); Py_TYPE(b)->tp_free(b); }
static PyMemberDef Box_members[] = {{"ref", T_OBJECT_EX, offsetof(Box, ref)}, {0}};
static PyType_Slot Box_slots[] = {
    {Py_tp_dealloc, Box_dealloc}, {Py_tp_members, Box_members}, {0, 0}
};
static PyType_Spec Box_spec = {"ext.Box", sizeof(Box), 0, Py_TPFLAGS_DEFAULT, Box_slots};

/* Meta is a metaclass that prints when one of its classes is created and freed. */
static PyObject *Meta_new(PyTypeObject *mt, PyObject *a, PyObject *k) {
    PyObject *t = PyType_Type.tp_new(mt, a, k);
    if (t) printf("%s allocated\n", ((PyTypeObject *) t)->tp_name);
    return t;
}
static void Meta_dealloc(PyObject *t) {
    printf("%s deleted\n", ((PyTypeObject *) t)->tp_name);
    PyType_Type.tp_dealloc(t);
}
static PyType_Slot Meta_slots[] = {
    {Py_tp_new, Meta_new}, {Py_tp_dealloc, Meta_dealloc}, {0, 0}
};
static PyType_Spec Meta_spec = {"ext.Meta", 0, 0, Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, Meta_slots};

static PyModuleDef def = {PyModuleDef_HEAD_INIT, "ext", NULL, -1};

PyMODINIT_FUNC PyInit_ext(void) {
    PyObject *m = PyModule_Create(&def);
    PyObject *bases = PyTuple_Pack(1, &PyType_Type);
    PyModule_AddObject(m, "Box", PyType_FromSpec(&Box_spec));
    PyModule_AddObject(m, "Meta", PyType_FromSpecWithBases(&Meta_spec, bases));
    Py_DECREF(bases);
    return m;
}
