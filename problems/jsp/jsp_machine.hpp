/**
 * @file jsp_machine.hpp
 * @author Pablo
 * @brief JSP Machine.
 * @version 0.1
 * @date 21-02-2019
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef JSPMACHINE_HPP_
#define JSPMACHINE_HPP_

#include <string>
#include <utility>

/**
 * @brief Machine of a JSP.
 *  
 */
class JSPMachine
{
  private:
    unsigned int machineID; // identifier of the machine

  public:
    /**
     * @brief Constructs a new JSPMachine.
     * 
     * @param machineID identifier of the machine.
     */
    JSPMachine(unsigned int machineID) : machineID{machineID} {}

    /**
     * @brief Returns the identifier of the machine.
     * 
     * @return the identifier of the machine.
     */
    unsigned int GetMachineID() const
    {
        return machineID;
    }

    bool operator==(const JSPMachine& other) const
    {
        return machineID == other.machineID;
    }

    bool operator!=(const JSPMachine& other) const
    {
        return machineID != other.machineID;
    }

    friend std::ostream& operator<<(std::ostream& os, const JSPMachine& machine)
    {
        return os << "{" << machine.machineID << "}";
    }
};

namespace std
{
    template <> struct hash<JSPMachine>
    {
        size_t operator()(const JSPMachine& k) const
        {
            return k.GetMachineID();
        }
    };
}

#endif /* JSPMACHINE_HPP_ */
