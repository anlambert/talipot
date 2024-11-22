/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/YajlFacade.h>

extern "C" {
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
}

YajlParseFacade::YajlParseFacade(tlp::PluginProgress *progress)
    : _progress(progress), _parsingSucceeded(true) {}

static int parse_null(void *ctx) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseNull();
  return 1;
}

static int parse_boolean(void *ctx, int boolVal) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseBoolean(boolVal);
  return 1;
}

static int parse_integer(void *ctx, long long integerVal) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseInteger(integerVal);
  return 1;
}

static int parse_double(void *ctx, double doubleVal) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseDouble(doubleVal);
  return 1;
}

// TODO check in which case this should be used instead of parse_integer and parse_double
// static int parse_number(void * ctx, const char * numberVal, size_t numberLen) {
//   YajlFacade* facade = (YajlFacade*) ctx;
//   facade->parseNumber(numberVal, numberLen);
//   return 1;
// }

static int parse_string(void *ctx, const unsigned char *stringVal, size_t stringLen) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  std::string key(reinterpret_cast<const char *>(stringVal), stringLen);
  facade->parseString(key);
  return 1;
}

static int parse_map_key(void *ctx, const unsigned char *stringVal, size_t stringLen) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  std::string key(reinterpret_cast<const char *>(stringVal), stringLen);
  facade->parseMapKey(key);
  return 1;
}

static int parse_start_map(void *ctx) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseStartMap();
  return 1;
}

static int parse_end_map(void *ctx) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseEndMap();
  return 1;
}

static int parse_start_array(void *ctx) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseStartArray();
  return 1;
}

static int parse_end_array(void *ctx) {
  auto *facade = static_cast<YajlParseFacade *>(ctx);
  facade->parseEndArray();
  return 1;
}

void YajlParseFacade::parse(const char *data, int length) {
  const yajl_callbacks callbacks = {parse_null,        parse_boolean,  parse_integer,
                                    parse_double,      nullptr,        parse_string,
                                    parse_start_map,   parse_map_key,  parse_end_map,
                                    parse_start_array, parse_end_array};
  yajl_handle hand = yajl_alloc(&callbacks, nullptr, this);
  yajl_status stat = yajl_parse(hand, reinterpret_cast<const unsigned char *>(data), length);

  if (stat != yajl_status_ok) {
    unsigned char *str =
        yajl_get_error(hand, 1, reinterpret_cast<const unsigned char *>(data), length);
    _parsingSucceeded = false;
    _errorMessage = std::string(reinterpret_cast<const char *>(str));
    yajl_free_error(hand, str);
  }

  yajl_free(hand);
}

bool YajlParseFacade::parsingSucceeded() const {
  return _parsingSucceeded;
}

std::string YajlParseFacade::errorMessage() const {
  return _errorMessage;
}

void YajlParseFacade::parseBoolean(bool) {}

void YajlParseFacade::parseDouble(double) {}

void YajlParseFacade::parseEndArray() {}

void YajlParseFacade::parseEndMap() {}

void YajlParseFacade::parseInteger(long long) {}

void YajlParseFacade::parseMapKey(const std::string &) {}

void YajlParseFacade::parseNull() {}

void YajlParseFacade::parseNumber(const char *, size_t) {}

void YajlParseFacade::parseStartArray() {}

void YajlParseFacade::parseStartMap() {}

void YajlParseFacade::parseString(const std::string &) {}

/** ============================================================= **/

YajlWriteFacade::YajlWriteFacade() {
  _generator = yajl_gen_alloc(nullptr);
  yajl_gen_config(_generator, yajl_gen_validate_utf8, 1);
}

void YajlWriteFacade::beautifyString(bool beautify) {
  yajl_gen_config(_generator, yajl_gen_beautify, beautify);

  if (beautify) {
    yajl_gen_config(_generator, yajl_gen_indent_string, "  ");
  } else {
    yajl_gen_config(_generator, yajl_gen_indent_string, "");
  }
}

YajlWriteFacade::~YajlWriteFacade() {
  yajl_gen_free(_generator);
}

void YajlWriteFacade::writeInteger(long long int number) {
  yajl_gen_integer(_generator, number);
}

void YajlWriteFacade::writeDouble(double number) {
  yajl_gen_double(_generator, number);
}

void YajlWriteFacade::writeNumber(const char *str, size_t len) {
  yajl_gen_number(_generator, str, len);
}

void YajlWriteFacade::writeString(const std::string &text) {
  yajl_gen_string(_generator, reinterpret_cast<const unsigned char *>(text.c_str()), text.size());
}

void YajlWriteFacade::writeNull() {
  yajl_gen_null(_generator);
}

void YajlWriteFacade::writeBool(bool value) {
  yajl_gen_bool(_generator, value);
}

void YajlWriteFacade::writeMapOpen() {
  yajl_gen_map_open(_generator);
}

void YajlWriteFacade::writeMapClose() {
  yajl_gen_map_close(_generator);
}

void YajlWriteFacade::writeArrayOpen() {
  yajl_gen_array_open(_generator);
}

void YajlWriteFacade::writeArrayClose() {
  yajl_gen_array_close(_generator);
}

std::string YajlWriteFacade::generatedString() {
  size_t length;
  const unsigned char *buffer;
  yajl_gen_status status = yajl_gen_get_buf(_generator, &buffer, &length);

  if (status != yajl_gen_status_ok) {
    tlp::debug() << __PRETTY_FUNCTION__ << ": parse error." << std::endl;
  }

  std::string result(reinterpret_cast<const char *>(buffer));
  return result;
}
