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

#ifndef TALIPOT_WITH_PARAMETER_H
#define TALIPOT_WITH_PARAMETER_H

#include <string>
#include <typeinfo>
#include <vector>

#include <talipot/config.h>

namespace tlp {

class Graph;
class DataSet;
template <class itType>
struct Iterator;

/**
 * @ingroup Plugins
 * @brief The ParameterDirection enum describes how a parameter is passed to a plugin
 * The parameter can be seen as passing parameters to a C++ functions and works as follow:
 * @list
 * @li IN_PARAM: The parameter is passes by value
 * @li OUT_PARAM: The parameter is a return value
 * @li INOUT_PARAM: The parameter is passed by reference.
 * @endlist
 */
enum ParameterDirection { IN_PARAM = 0, OUT_PARAM = 1, INOUT_PARAM = 2 };

/**
 * @ingroup Plugins
 * @brief Describes a plugin's parameter.
 *
 * A plugin parameter consists of the following information:
 * @list
 * @li A name (std::string) which can be used to retrieve the value of the parameter when running
 *the plugin.
 * @li A type (std::string) which is the C++ typename of the parameter.
 * @li A help string (std::string) which gives additional information about the parameter and its
 *possible values.
 * @li A default value which is mapped on the parameter if no value has been entered by the user.
 * @li The mandatory flag (bool) which tells if the parameter is optional or not.
 * @li A ParameterDirection (enum).
 * @endlist
 *
 * @see tlp::ParameterDirection
 * @see tlp::ParameterDescriptionList
 * @see tlp::WithParameter
 **/
class TLP_SCOPE ParameterDescription {
  private:
    std::string name;
    std::string type;
    std::string help;
    std::string defaultValue;
    bool mandatory;
    ParameterDirection direction;

  public:
    ParameterDescription() = default;
    ParameterDescription(const std::string &name, const std::string &type, const std::string &help,
                         const std::string &defaultValue, bool mandatory,
                         ParameterDirection direction)
        : name(name), type(type), help(help), defaultValue(defaultValue), mandatory(mandatory),
          direction(direction) {}

    /**
     * @return The parameter's name
     */
    const std::string &getName() const {
        return name;
    }
    /**
     * @return The parameter's C++ type name
     */
    const std::string &getTypeName() const {
        return type;
    }
    /**
     * @return The parameter's help string
     */
    const std::string &getHelp() const {
        return help;
    }
    /**
     * @return The parameter's default value.
     */
    const std::string &getDefaultValue() const {
        return defaultValue;
    }
    /**
     * @brief Set the parameter's default value.
     */
    void setDefaultValue(const std::string &defVal) {
        defaultValue = defVal;
    }
    /**
     * @return Whether the parameter is mandatory or not.
     */
    bool isMandatory() const {
        return mandatory;
    }
    /**
     * @return The parameter's direction
     */
    ParameterDirection getDirection() const {
        return direction;
    }
    /**
     * @return Set the parameter's direction
     */
    void setDirection(ParameterDirection dir) {
        direction = dir;
    }
    /**
     * @return Whether the parameter is editable or not.
     */
    bool isEditable() const {
        // input parameters are editable
        return (direction != OUT_PARAM) ||
               // only property output parameters are editable
               (type.find("Property") != std::string::npos);
    }
};

/**
 * @ingroup Plugins
 * @brief This class describes parameters taken by a plugin.
 *
 * It is used by WithParameter to store parameters.
 * Each parameter is identified by a name, has a default value, a value, a help string, and a
 *boolean indicating whether it is mandatory or optional.
 **/
struct TLP_SCOPE ParameterDescriptionList {

    ParameterDescriptionList() = default;

    /**
     * @brief Adds a new parameter of type T to the list.
     *
     * @param parameterName The name of the parameter.
     * @param help The help string of this parameter. Defaults to std::string().
     * @param defaultValue The default value of this parameter. Defaults to std::string().
     * @param isMandatory Whether this parameter is mandatory or optional. Defaults to true.
     * @param direction The parameter's direction (see tlp::ParameterDirection for details)
     * @return void
     **/
    template <typename T>
    void add(const std::string &parameterName, const std::string &help,
             const std::string &defaultValue, bool isMandatory = true,
             ParameterDirection direction = IN_PARAM,
             const std::string &valuesDescription = std::string()) {
        for (const auto &parameter : parameters) {
            if (parameter.getName() == parameterName) {
#ifndef NDEBUG
                tlp::warning() << "ParameterDescriptionList::add " << parameterName
                               << " already exists" << std::endl;
#endif
                return;
            }
        }

        ParameterDescription newParameter(
            parameterName, typeid(T).name(),
            generateParameterHTMLDocumentation(parameterName, help, typeid(T).name(), defaultValue,
                                               valuesDescription, direction),
            defaultValue, isMandatory, direction);
        parameters.push_back(newParameter);
    }

    /**
     * @brief Retrieves an Iterator on the parameters.
     *
     * @return An iterator over the parameters :Iterator<ParameterDescription>*
     **/
    tlp::Iterator<ParameterDescription> *getParameters() const;

    /**
     * @brief retrieves the default value of a parameter.
     *
     * @param parameterName The name of the parameter to retrieve the default value of.
     * @return The default value of the parameter:string
     **/
    const std::string &getDefaultValue(const std::string &parameterName) const;

    /**
     * @brief Sets the default value of a parameter.
     *
     * @param parameterName The name of the parameter to set the value of.
     * @param value The new value for this parameter.
     * @return void
     **/
    void setDefaultValue(const std::string &parameterName, const std::string &value);

    /**
     * @brief Sets the direction of a parameter.
     *
     * @param parameterName The name of the parameter to set the value of.
     * @param dir The new direction for this parameter.
     * @return void
     **/
    void setDirection(const std::string &parameterName, ParameterDirection direction);

    /**
     * @brief Retrieves whether a parameter is mandatory or optional.
     *
     * @param parameterName The name of the parameter for which to check if it is mandatory or
     *optional.
     * @return bool true if the parameter is mandatory, false if it is optional.
     **/
    bool isMandatory(const std::string &parameterName) const;

    /**
     * @brief Builds a DataSet containing the default values for each parameter.
     * If the DataSet has a key which is equal to a parameter's name, the existing value is kept.
     *
     * @param ioDataSet The input dataset on which the parameters names will be associated with
     *their default values.
     * @param inG A graph on which to create properties if any parameter is of a property type.
     *Defaults to 0.
     * @return void
     **/
    void buildDefaultDataSet(DataSet &ioDataSet, Graph *inG = nullptr) const;

    /**
     * @brief Returns the number of parameters.
     *
     * @return The number of parameters
     **/
    uint size() const {
        return parameters.size();
    }

    /**
     * @brief Test if the list is empty
     *
     * @return bool true if the parameter description list is empty, false otherwise.
     **/
    bool empty() const {
        return parameters.empty();
    }

  private:
    ParameterDescription *getParameter(const std::string &parameterName);
    std::string generateParameterHTMLDocumentation(const std::string &name, const std::string &help,
                                                   const std::string &type,
                                                   const std::string &defaultValue,
                                                   const std::string &valuesDescription,
                                                   const ParameterDirection &direction);
    std::vector<ParameterDescription> parameters;
};

/**
 * @ingroup Plugins
 * @brief This class describes parameters on a plug-in.
 *
 * These parameters can be of any type, and are used to generate a GUI that will be shown when the
 *plug-in in invoked by the user.
 * It is mainly used by algorithms to display options to the user, e.g. a clustering algorithm can
 *let the user choose which measure to use.
 **/
struct TLP_SCOPE WithParameter {

    /**
     * @brief Retrieves the parameters.
     *
     * @return :ParameterDescriptionList the parameters of the plug-in.
     **/
    const tlp::ParameterDescriptionList &getParameters() const;

    /**
     * @brief Adds an IN parameter to the plug-in.
     *
     * @param name The name of the parameter to add.
     * @param help A description of the parameter, that will be displayed to the user. Defaults to
     *"".
     * @param defaultValue The default value the parameter should take, to be the initial value in
     *the GUI. Defaults to "".
     * @param isMandatory Whether this parameter requires a value or not. Defaults to true.
     * @param valuesDescription A description of the values the parameter can take (usually for
     *detailing the content of a StringCollection). Defaults to "".
     * @return void
     **/
    template <typename T>
    void addInParameter(const std::string &name, const std::string &help,
                        const std::string &defaultValue, bool isMandatory = true,
                        const std::string &valuesDescription = std::string()) {
        parameters.template add<T>(name, help, defaultValue, isMandatory, IN_PARAM,
                                   valuesDescription);
    }

    /**
     * @brief Adds an OUT parameter to the plug-in.
     *
     * @param name The name of the parameter to add.
     * @param help A description of the parameter, that will be displayed to the user. Defaults to
     *"".
     * @param defaultValue The default value the parameter should take, to be the initial value in
     *the GUI. Defaults to "".
     * @param isMandatory Whether this parameter requires a value or not. Defaults to true.
     * @param valuesDescription A description of the values the parameter can take (usually for
     *detailing the content of a StringCollection). Defaults to "".
     * @return void
     **/
    template <typename T>
    void addOutParameter(const std::string &name, const std::string &help = std::string(),
                         const std::string &defaultValue = std::string(), bool isMandatory = true,
                         const std::string &valuesDescription = std::string()) {
        parameters.template add<T>(name, help, defaultValue, isMandatory, OUT_PARAM,
                                   valuesDescription);
    }

    /**
     * @brief Adds an INOUT parameter to the plug-in.
     *
     * @param name The name of the parameter to add.
     * @param help A description of the parameter, that will be displayed to the user. Defaults to
     *"".
     * @param defaultValue The default value the parameter should take, to be the initial value in
     *the GUI. Defaults to "".
     * @param isMandatory Whether this parameter requires a value or not. Defaults to true.
     * @param valuesDescription A description of the values the parameter can take (usually for
     *detailing the content of a StringCollection). Defaults to "".
     * @return void
     **/
    template <typename T>
    void addInOutParameter(const std::string &name, const std::string &help = std::string(),
                           const std::string &defaultValue = std::string(), bool isMandatory = true,
                           const std::string &valuesDescription = std::string()) {
        parameters.template add<T>(name, help, defaultValue, isMandatory, INOUT_PARAM,
                                   valuesDescription);
    }

    /**
     * @brief indicates whether the embedded parameters require some user input
     *
     * @return true if an input parameter or a property output parameter exists
     **/
    bool inputRequired() const;

  protected:
    /**
     * @brief The internal structure storing the parameters.
     **/
    ParameterDescriptionList parameters;
};
}
#endif // TALIPOT_WITH_PARAMETER_H
