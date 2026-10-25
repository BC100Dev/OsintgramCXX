#ifndef DSI_DEVICELISTING_HPP
#define DSI_DEVICELISTING_HPP

#include <vector>
#include <string>

#include <IGApi/Session.hpp>

namespace DSI {

    std::vector<IG::Session::Device> GetDeviceListing();

    std::vector<IG::Session::Device> GetDeviceListing(bool local);

    void SaveDeviceListing(const std::vector<IG::Session::Device>& deviceList);

    int Update();

    void ContributeHostDevice();

}

#endif //DSI_DEVICELISTING_HPP
