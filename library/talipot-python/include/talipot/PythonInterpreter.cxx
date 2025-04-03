/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

TLP_PYTHON_SCOPE void decrefPyObject(PyObject *obj);

template <typename T>
bool PythonInterpreter::evalSingleStatementAndGetValue(const QString &pythonStatement, T &value) {
    holdGIL();

    PyObject *ret = evalPythonStatement(pythonStatement);

    bool ok = false;

    if (ret) {
        PyObjectToCppObjectConverter<T> converter;
        ok = converter.convert(ret, value);
        decrefPyObject(ret);
    }

    releaseGIL();

    return ok;
}

template <typename RETURN_TYPE, typename... Param>
bool PythonInterpreter::callFunctionWithParamsAndGetReturnValue(const QString &module,
                                                                const QString &function,
                                                                RETURN_TYPE &returnValue,
                                                                Param... param) {
    tlp::DataSet ds;
    buildParamDataSet(&ds, param...);
    return callFunctionAndGetReturnValue(module, function, ds, returnValue);
}

template <typename... Param>
bool PythonInterpreter::callFunctionWithParams(const QString &module, const QString &function,
                                               Param... param) {
    tlp::DataSet ds;
    buildParamDataSet(&ds, param...);
    return callFunction(module, function, ds);
}

template <typename T, typename... Param>
void PythonInterpreter::buildParamDataSet(DataSet *ds, T a, Param... param) {
    addParameter(ds, a);
    buildParamDataSet(ds, param...);
}

template <typename T>
void PythonInterpreter::buildParamDataSet(DataSet *ds, T a) {
    addParameter(ds, a);
}

template <typename T>
void PythonInterpreter::addParameter(DataSet *ds, T a) {
    std::string st("param_");
    st += std::to_string(ds->size() + 1);
    ds->set(st, a);
}

template <typename RETURN_TYPE>
bool PythonInterpreter::callFunctionAndGetReturnValue(const QString &module,
                                                      const QString &function,
                                                      const tlp::DataSet &parameters,
                                                      RETURN_TYPE &returnValue) {
    holdGIL();
    bool ok = false;
    PyObject *ret = callPythonFunction(module, function, parameters);
    PyObjectToCppObjectConverter<RETURN_TYPE> retConverter;

    if (ret && retConverter.convert(ret, returnValue)) {
        ok = true;
    }

    decrefPyObject(ret);
    releaseGIL();
    return ok;
}
