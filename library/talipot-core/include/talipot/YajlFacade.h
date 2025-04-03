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

#include <string>

#include <talipot/config.h>

namespace tlp {
class PluginProgress;
}

struct yajl_gen_t;

/**
 * @brief A Simple C++ wrapper around the C library 'yajl' parsing capabilities.
 *
 * It uses callbacks to signal what is being parsed (map start and end, strings, ...), and this
 *class is intended to be subclassed,
 * with the subclass overriding the callbacks to process the events.
 **/
class TLP_SCOPE YajlParseFacade {
  public:
    YajlParseFacade(tlp::PluginProgress *progress = nullptr);

    virtual ~YajlParseFacade() = default;
    /**
     * @brief Parses JSON data.
     * Once this function is called, the callbacks (all the parse*
     * functions) will get called when the corresponding event happens.
     *
     * @param jsonData JSON data to parse
     * @param length JSON data length in bytes
     **/
    void parse(const char *jsonData, int length);

    virtual void parseNull();
    virtual void parseBoolean(bool boolVal);
    virtual void parseInteger(long long integerVal);
    virtual void parseDouble(double doubleVal);
    virtual void parseNumber(const char *numberVal, size_t numberLen);
    virtual void parseString(const std::string &value);
    virtual void parseMapKey(const std::string &value);
    virtual void parseStartMap();
    virtual void parseEndMap();
    virtual void parseStartArray();
    virtual void parseEndArray();

    bool parsingSucceeded() const;
    std::string errorMessage() const;

  protected:
    tlp::PluginProgress *_progress;
    bool _parsingSucceeded;
    std::string _errorMessage;
};

class YajlWriteFacade {
  public:
    YajlWriteFacade();
    ~YajlWriteFacade();

    std::string generatedString();

    void writeInteger(long long int number);
    void writeDouble(double number);
    void writeNumber(const char *str, size_t len);
    void writeString(const std::string &text);
    void writeNull();
    void writeBool(bool value);
    void writeMapOpen();
    void writeMapClose();
    void writeArrayOpen();
    void writeArrayClose();

    void beautifyString(bool beautify);

  protected:
    yajl_gen_t *_generator;
};
