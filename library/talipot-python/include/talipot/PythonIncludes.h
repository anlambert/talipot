/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PYTHON_INCLUDES_H
#define TALIPOT_PYTHON_INCLUDES_H

// Need to include cmath before Python.h when compiling with MinGW and C++11 standard
// to avoid a compilation error (see http://stackoverflow.com/questions/28683358/)
#if defined(__MINGW32__)
#include <math.h>
#include <cmath>
#endif

// thanks to the VTK project for this patch for Visual Studio in debug mode
#if defined(_MSC_VER) && defined(_DEBUG)
// Include these low level headers before undefing _DEBUG. Otherwise when doing
// a debug build against a release build of python the compiler will end up
// including these low level headers without DEBUG enabled, causing it to try
// and link release versions of this low level C api.
#include <basetsd.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <io.h>
#include <math.h>
#include <sal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#undef _DEBUG
#if _MSC_VER >= 1400
#define _CRT_NOFORCE_MANIFEST 1
#endif
#include <Python.h>
#include <frameobject.h>
#include <structmember.h>
#include <import.h>
#include <sip.h>
#define _DEBUG
#else
#include <Python.h>
#include <frameobject.h>
#include <structmember.h>
#include <import.h>
#include <sip.h>
#endif

#include <talipot/config.h>

static const sipAPIDef *getSipAPI() {
  return static_cast<const sipAPIDef *>(PyCapsule_Import("talipot.native.sip._C_API", 0));
}

static const sipAPIDef *sipAPIPtr = nullptr;

inline const sipAPIDef *sipAPI() {
  if (!sipAPIPtr) {
    sipAPIPtr = getSipAPI();
  }

  return sipAPIPtr;
}

#define sipBadCallableArg sipAPI()->api_bad_callable_arg
#define sipBadCatcherResult sipAPI()->api_bad_catcher_result
#define sipBadLengthForSlice sipAPI()->api_bad_length_for_slice
#define sipBuildResult sipAPI()->api_build_result
#define sipCallMethod sipAPI()->api_call_method
#define sipCallProcedureMethod sipAPI()->api_call_procedure_method
#define sipCanConvertToType sipAPI()->api_can_convert_to_type
#define sipCheckPluginForType sipAPI()->api_check_plugin_for_type
#define sipConvertFromConstVoidPtr sipAPI()->api_convert_from_const_void_ptr
#define sipConvertFromConstVoidPtrAndSize sipAPI()->api_convert_from_const_void_ptr_and_size
#define sipConvertFromEnum sipAPI()->api_convert_from_enum
#define sipConvertFromNewPytype sipAPI()->api_convert_from_new_pytype
#define sipConvertFromNewType sipAPI()->api_convert_from_new_type
#define sipConvertFromSequenceIndex sipAPI()->api_convert_from_sequence_index
#define sipConvertFromSliceObject sipAPI()->api_convert_from_slice_object
#define sipConvertFromType sipAPI()->api_convert_from_type
#define sipConvertFromVoidPtr sipAPI()->api_convert_from_void_ptr
#define sipConvertFromVoidPtrAndSize sipAPI()->api_convert_from_void_ptr_and_size
#define sipConvertToArray sipAPI()->api_convert_to_array
#define sipConvertToBool sipAPI()->api_convert_to_bool
#define sipConvertToEnum sipAPI()->api_convert_to_enum
#define sipConvertToType sipAPI()->api_convert_to_type
#define sipConvertToTypeUs sipAPI()->api_convert_to_type_us
#define sipConvertToTypedArray sipAPI()->api_convert_to_typed_array
#define sipConvertToVoidPtr sipAPI()->api_convert_to_void_ptr
#define sipEnableAutoconversion sipAPI()->api_enable_autoconversion
#define sipEnableGc sipAPI()->api_enable_gc
#define sipExportSymbol sipAPI()->api_export_symbol
#define sipFindType sipAPI()->api_find_type
#define sipForceConvertToType sipAPI()->api_force_convert_to_type
#define sipForceConvertToTypeUs sipAPI()->api_force_convert_to_type_us
#define sipFree sipAPI()->api_free
#define sipFromDate sipAPI()->api_from_date
#define sipFromDatetime sipAPI()->api_from_datetime
#define sipFromMethod sipAPI()->api_from_method
#define sipFromTime sipAPI()->api_from_time
#define sipGetAddress sipAPI()->api_get_address
#define sipGetBufferInfo sipAPI()->api_get_buffer_info
#define sipGetCFunction sipAPI()->api_get_c_function
#define sipGetDate sipAPI()->api_get_date
#define sipGetDatetime sipAPI()->api_get_datetime
#define sipGetInterpreter sipAPI()->api_get_interpreter
#define sipGetMethod sipAPI()->api_get_method
#define sipGetMixinAddress sipAPI()->api_get_mixin_address
#define sipGetPyobject sipAPI()->api_get_pyobject
#define sipGetState sipAPI()->api_get_state
#define sipGetTime sipAPI()->api_get_time
#define sipGetTypeUserData sipAPI()->api_get_type_user_data
#define sipGetUserObject sipAPI()->api_get_user_object
#define sipImportSymbol sipAPI()->api_import_symbol
#define sipInstanceDestroyed sipAPI()->api_instance_destroyed
#define sipIsEnumFlag sipAPI()->api_is_enum_flag
#define sipIsOwnedByPython sipAPI()->api_is_owned_by_python
#define sipIsUserType sipAPI()->api_is_user_type
#define sipLongAsChar sipAPI()->api_long_as_char
#define sipLongAsInt sipAPI()->api_long_as_int
#define sipLongAsLong sipAPI()->api_long_as_long
#define sipLongAsLongLong sipAPI()->api_long_as_long_long
#define sipLongAsShort sipAPI()->api_long_as_short
#define sipLongAsSignedChar sipAPI()->api_long_as_signed_char
#define sipLongAsSizeT sipAPI()->api_long_as_size_t
#define sipLongAsUnsignedChar sipAPI()->api_long_as_unsigned_char
#define sipLongAsUnsignedInt sipAPI()->api_long_as_unsigned_int
#define sipLongAsUnsignedLong sipAPI()->api_long_as_unsigned_long
#define sipLongAsUnsignedLongLong sipAPI()->api_long_as_unsigned_long_long
#define sipLongAsUnsignedShort sipAPI()->api_long_as_unsigned_short
#define sipMalloc sipAPI()->api_malloc
#define sipParseResult sipAPI()->api_parse_result
#define sipPrintObject sipAPI()->api_print_object
#define sipPyTypeDict sipAPI()->api_py_type_dict
#define sipPyTypeName sipAPI()->api_py_type_name
#define sipRegisterAttributeGetter sipAPI()->api_register_attribute_getter
#define sipRegisterEventHandler sipAPI()->api_register_event_handler
#define sipRegisterExitNotifier sipAPI()->api_register_exit_notifier
#define sipRegisterProxyResolver sipAPI()->api_register_proxy_resolver
#define sipRegisterPyType sipAPI()->api_register_py_type
#define sipReleaseBufferInfo sipAPI()->api_release_buffer_info
#define sipReleaseType sipAPI()->api_release_type
#define sipReleaseTypeUs sipAPI()->api_release_type_us
#define sipResolveTypedef sipAPI()->api_resolve_typedef
#define sipSetTypeUserData sipAPI()->api_set_type_user_data
#define sipSetUserObject sipAPI()->api_set_user_object
#define sipTrace sipAPI()->api_trace
#define sipTransferBack sipAPI()->api_transfer_back
#define sipTransferTo sipAPI()->api_transfer_to
#define sipTypeFromPyTypeObject sipAPI()->api_type_from_py_type_object
#define sipTypeScope sipAPI()->api_type_scope
#define sipUnicodeData sipAPI()->api_unicode_data
#define sipUnicodeNew sipAPI()->api_unicode_new
#define sipUnicodeWrite sipAPI()->api_unicode_write
#define sipVisitWrappers sipAPI()->api_visit_wrappers

#endif // TALIPOT_PYTHON_INCLUDES_H
